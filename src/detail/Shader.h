#pragma once

#include <glad/glad.h>
#include <fstream>
#include <string>

namespace ui::detail {

    struct Shader {
        unsigned int ID;

        Shader(const char* path, GLenum type) {
            ID = glCreateShader(type);

            std::ifstream R(path);
            if (!R.is_open()) {
                printf("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
            }
            const std::string content(
                (std::istreambuf_iterator<char>(R)), std::istreambuf_iterator<char>()
            );
            R.close();


            const char* code = content.c_str();
            glShaderSource(ID, 1, &code, NULL);
            glCompileShader(ID);


            int success;
            char infoLog[512];
            glGetShaderiv(ID, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(ID, 512, NULL, infoLog);
                printf("ERROR::SHADER::COMPILATION_FAILED::%s\n%s", path, infoLog);
            }
            printf("Shader %s compiled %d\n", path, ID);
        }

        ~Shader() { glDeleteShader(ID); }
    };
}  // namespace ui::detail