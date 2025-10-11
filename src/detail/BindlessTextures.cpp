#include "BindlessTextures.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace ui;

BindlessTextures::BindlessTextures() {
    glGenBuffers(1, &m_ssboID);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssboID);
    update();
}

size_t BindlessTextures::add(
    unsigned char* data, int width, int height, int channels, bool pixelated
) {
    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pixelated ? GL_NEAREST : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(glm::vec4(0, 0, 0, 0)));


    GLenum format = channels == 1   ? GL_RED
                    : channels == 2 ? GL_RG
                    : channels == 3 ? GL_RGB
                                    : GL_RGBA;

    GLenum internalFormat = channels == 1   ? GL_R8
                            : channels == 2 ? GL_RG8
                            : channels == 3 ? GL_RGB8
                                            : GL_RGBA8;

    printf(
        "Creating texture: texID=%u, format=%s, data=%p\n",
        tex,
        channels == 1   ? "RED"
        : channels == 2 ? "RG"
        : channels == 3 ? "RGB"
                        : "RGBA",
        data
    );

    glTexImage2D(
        GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data
    );

    GLuint64 handle = glGetTextureHandleARB(tex);
    //glMakeTextureHandleResidentARB(handle);

    m_textureHandles.push_back(handle);
    m_textures.push_back(tex);

    m_dirty = true;

    return m_textures.size() - 1;
}

void BindlessTextures::use() {
    for (int i = 0; i < m_textureHandles.size(); i++) {
        glMakeTextureHandleResidentARB(m_textureHandles[i]);
    }
}

void BindlessTextures::use(int index) {
    glMakeTextureHandleResidentARB(m_textureHandles[index]);
}

void BindlessTextures::unload() {
    for (int i = 0; i < m_textureHandles.size(); i++) {
        glMakeTextureHandleNonResidentARB(m_textureHandles[i]);
    }
}

void BindlessTextures::unload(int index) {
    glMakeTextureHandleNonResidentARB(m_textureHandles[index]);
}

void BindlessTextures::destroy(int index) {
    unload(index);
    glDeleteTextures(1, &m_textures[index]);
    m_textureHandles.erase(m_textureHandles.begin() + index);
    m_textures.erase(m_textures.begin() + index);
    m_dirty = true;
}

void BindlessTextures::setUniform(const char* name, int index, unsigned int programID) {
    unsigned int location = glGetUniformLocation(programID, name);
    glUniformHandleui64ARB(location, m_textureHandles[index]);
}

void BindlessTextures::update() {
    if (m_dirty) {
        glNamedBufferData(
            m_ssboID,
            sizeof(GLuint64) * m_textureHandles.size(),
            m_textureHandles.data(),
            GL_DYNAMIC_DRAW
        );
        m_dirty = false;
    }
}