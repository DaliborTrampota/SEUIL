#pragma once

#include <msdf-atlas-gen/AtlasGenerator.h>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>


using namespace msdf_atlas;

template <typename T, int N, GeneratorFunction<T, N> GEN_FN, class AtlasStorage>
class ThreadPoolAtlasGenerator {
  public:
    ThreadPoolAtlasGenerator();
    ThreadPoolAtlasGenerator(int width, int height);
    template <typename... ARGS>
    ThreadPoolAtlasGenerator(int width, int height, ARGS... storageArgs);

    // Delete copy, custom move (mutex/condition_variable not movable)
    ThreadPoolAtlasGenerator(const ThreadPoolAtlasGenerator&) = delete;
    ThreadPoolAtlasGenerator& operator=(const ThreadPoolAtlasGenerator&) = delete;
    ThreadPoolAtlasGenerator(ThreadPoolAtlasGenerator&&) noexcept;
    ThreadPoolAtlasGenerator& operator=(ThreadPoolAtlasGenerator&&) noexcept;
    ~ThreadPoolAtlasGenerator();

    void generate(const GlyphGeometry* glyphs, int count);
    void rearrange(int width, int height, const Remap* remapping, int count);
    void resize(int width, int height);
    /// Sets attributes for the generator function
    void setAttributes(const GeneratorAttributes& attributes);
    /// Sets the number of threads to be run by generate
    void setThreadCount(int threadCount);
    /// Allows access to the underlying AtlasStorage
    const AtlasStorage& atlasStorage() const;
    /// Returns the layout of the contained glyphs as a list of GlyphBoxes
    const std::vector<GlyphBox>& getLayout() const;

  private:
    using Job = std::function<bool(int)>;

    void shutdownThreads();
    void startThreads();
    void loop(int threadNo);

    AtlasStorage storage;
    std::vector<GlyphBox> layout;
    std::vector<T> glyphBuffer;
    std::vector<byte> errorCorrectionBuffer;
    GeneratorAttributes attributes;

    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::condition_variable m_completeCv;

    std::vector<std::thread> m_threads;
    std::queue<Job> m_jobs;
    int m_threadCount = 2;
    int m_activeJobs = 0;
    bool m_terminate = false;
};

#include "ThreadPoolAtlasGenerator.hpp"