
#include "ThreadPoolAtlasGenerator.h"

#include <algorithm>

using namespace msdf_atlas;

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::ThreadPoolAtlasGenerator() {}


template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::ThreadPoolAtlasGenerator(int width, int height)
    : storage(width, height) {}


template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
template <typename... ARGS>
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::ThreadPoolAtlasGenerator(
    int width, int height, ARGS... storageArgs
)
    : storage(width, height, storageArgs...) {}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::ThreadPoolAtlasGenerator(
    ThreadPoolAtlasGenerator&& other
) noexcept
    : storage(std::move(other.storage)),
      layout(std::move(other.layout)),
      glyphBuffer(std::move(other.glyphBuffer)),
      errorCorrectionBuffer(std::move(other.errorCorrectionBuffer)),
      attributes(std::move(other.attributes)) {
    // Don't move threads - they have dangling 'this' pointers
    // Threads will be lazily initialized on first generate() call
    // Shutdown the other object's threads
    other.shutdownThreads();
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>&
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::operator=(
    ThreadPoolAtlasGenerator&& other
) noexcept {
    if (this != &other) {
        shutdownThreads();  // Clean up our threads first

        storage = std::move(other.storage);
        layout = std::move(other.layout);
        glyphBuffer = std::move(other.glyphBuffer);
        errorCorrectionBuffer = std::move(other.errorCorrectionBuffer);
        attributes = std::move(other.attributes);
        // Don't move threads or jobs - will be reinitialized on next generate()

        other.shutdownThreads();
    }
    return *this;
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::~ThreadPoolAtlasGenerator() {
    shutdownThreads();
}


template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::shutdownThreads() {
    {
        std::unique_lock lock(m_mutex);
        m_terminate = true;
    }
    m_cv.notify_all();

    for (auto& thread : m_threads) {
        if (thread.joinable())
            thread.join();
    }

    // Clear any remaining jobs
    while (!m_jobs.empty())
        m_jobs.pop();

    m_terminate = false;
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::startThreads() {
    if (!m_threads.empty())
        return;

    m_threads.resize(m_threadCount);
    for (int i = 0; i < m_threadCount; ++i) {
        m_threads[i] =
            std::thread(&ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::loop, this, i);
    }
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::generate(
    const GlyphGeometry* glyphs, int count
) {
    int maxBoxArea = 0;
    for (int i = 0; i < count; ++i) {
        GlyphBox box = glyphs[i];
        maxBoxArea = std::max(maxBoxArea, box.rect.w * box.rect.h);
        layout.push_back((GlyphBox&&)box);
    }
    int threadBufferSize = N * maxBoxArea;
    if (m_threadCount * threadBufferSize > (int)glyphBuffer.size())
        glyphBuffer.resize(m_threadCount * threadBufferSize);
    if (m_threadCount * maxBoxArea > (int)errorCorrectionBuffer.size())
        errorCorrectionBuffer.resize(m_threadCount * maxBoxArea);
    std::vector<GeneratorAttributes> threadAttributes(m_threadCount);
    for (int i = 0; i < m_threadCount; ++i) {
        threadAttributes[i] = attributes;
        threadAttributes[i].config.errorCorrection.buffer =
            errorCorrectionBuffer.data() + i * maxBoxArea;
    }

    startThreads();


    int batchSize = std::max(1, static_cast<int>(count / (m_threadCount * 4)));
    int batchCount = (count + batchSize - 1) / batchSize;

    for (int _batch = 0; _batch < batchCount; ++_batch) {
        int start = _batch * batchSize;
        int end = std::min(start + batchSize, count);
        Job gen =
            [this, glyphs, &threadAttributes, threadBufferSize, start, end](int threadNo) -> bool {
            for (int i = start; i < end; ++i) {
                const GlyphGeometry& glyph = glyphs[i];
                if (!glyph.isWhitespace()) {
                    int l, b, w, h;
                    glyph.getBoxRect(l, b, w, h);
                    msdfgen::BitmapRef<T, N> glyphBitmap(
                        glyphBuffer.data() + threadNo * threadBufferSize, w, h
                    );
                    GEN_FN(glyphBitmap, glyph, threadAttributes[threadNo]);
                    storage.put(l, b, msdfgen::BitmapConstRef<T, N>(glyphBitmap));
                }
            }
            return true;
        };
        m_jobs.push(gen);
    }
    if (batchSize == 1)
        m_cv.notify_one();
    else
        m_cv.notify_all();

    // Wait for all jobs to complete
    {
        std::unique_lock lock(m_mutex);
        m_completeCv.wait(lock, [this] { return m_jobs.empty() && m_activeJobs == 0; });
    }
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::rearrange(
    int width, int height, const Remap* remapping, int count
) {
    for (int i = 0; i < count; ++i) {
        layout[remapping[i].index].rect.x = remapping[i].target.x;
        layout[remapping[i].index].rect.y = remapping[i].target.y;
    }
    AtlasStorage newStorage((AtlasStorage&&)storage, width, height, remapping, count);
    storage = (AtlasStorage&&)newStorage;
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::resize(int width, int height) {
    AtlasStorage newStorage((AtlasStorage&&)storage, width, height);
    storage = (AtlasStorage&&)newStorage;
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::setAttributes(
    const GeneratorAttributes& attributes
) {
    this->attributes = attributes;
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::setThreadCount(int threadCount) {
    m_threadCount = 2;
    // TODO relaunch threads
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
const AtlasStorage& ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::atlasStorage() const {
    return storage;
}

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
const std::vector<GlyphBox>&
ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::getLayout() const {
    return layout;
}


template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
void ThreadPoolAtlasGenerator<T, N, GEN_FN, AtlasStorage>::loop(int threadNo) {
    while (true) {
        Job job;
        {
            std::unique_lock lock(m_mutex);
            m_cv.wait(lock, [this] { return !m_jobs.empty() || m_terminate; });

            if (m_terminate)
                break;

            job = m_jobs.front();
            m_jobs.pop();
            m_activeJobs++;
        }
        job(threadNo);

        {
            std::unique_lock lock(m_mutex);
            m_activeJobs--;
            if (m_jobs.empty() && m_activeJobs == 0)
                m_completeCv.notify_all();
        }
    }
}