#pragma once

#include "core/defines.hpp"
#include <glad/glad.h>
#include <sstream>
#include <vector>

class Shader
{
private:
    GLuint m_program;
    std::vector<string> splitShaderSources(const char *glslCode);
public:
    Shader(const char *glslCode);
    ~Shader();
    void use();
};
