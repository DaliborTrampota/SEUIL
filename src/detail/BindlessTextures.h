#pragma once

#include <string>
#include <vector>


namespace ui {

    class BindlessTextures {
      public:
        BindlessTextures();

        size_t add(unsigned char* data, int width, int height, int channels, bool pixelated);

        void use();
        void use(int index);

        void unload();
        void unload(int index);

        void destroy(int index);
        void update();

        void setUniform(const char* name, int index, unsigned int programID);

      private:
        std::vector<unsigned int> m_textures;
        std::vector<unsigned long long> m_textureHandles;
        bool m_dirty = false;

        unsigned int m_ssboID = 0;
    };
}  // namespace ui