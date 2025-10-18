#pragma once

#include <vector>

#include <glad/glad.h>

namespace ui::detail {

    template <typename T>
    class SSBO {
      public:
        SSBO(unsigned int bindingPoint) : m_bindingPoint(bindingPoint) {}
        ~SSBO() {
            if (m_id != 0) {
                glDeleteBuffers(1, &m_id);
            }
        }

        void create(std::vector<T>&& data) {
            glGenBuffers(1, &m_id);
            if (data.size()) {
                glNamedBufferData(m_id, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
                m_data = std::move(data);
            }
        }
        void bind() {
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bindingPoint, m_id);
            if (m_dirty) {
                glNamedBufferData(m_id, m_data.size() * sizeof(T), m_data.data(), GL_STATIC_DRAW);
                m_dirty = false;
            }
        }
        void unbind() { glBindBufferBase(GL_SHADER_STORAGE_BUFFER, m_bindingPoint, 0); }

        void clear() {
            m_data.clear();
            m_dirty = true;
        }

        void reserve(size_t size) { m_data.reserve(size); }

        void setData(std::vector<T>&& data) {
            m_data = std::move(data);
            m_dirty = true;
        }

        void add(T data) {
            m_data.push_back(data);
            m_dirty = true;
        }

        const std::vector<T>& data() const { return m_data; }

      private:
        unsigned int m_id = 0;
        unsigned int m_bindingPoint;

        std::vector<T> m_data;
        bool m_dirty = false;
    };
}  // namespace ui::detail