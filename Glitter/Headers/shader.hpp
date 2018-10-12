#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace shared_obj {

    class Shader {
    public:
        GLuint Program;

        // Constructor generates the shader on the fly
        Shader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath = nullptr);

        // Uses the current shader
        void Use();

    private:
        void checkCompileErrors(GLuint shader, std::string type, const char *file_path);
    };

}  // namespace shared_obj

#endif