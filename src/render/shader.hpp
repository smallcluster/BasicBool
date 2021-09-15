#pragma once

#include "core/defines.hpp"
#include <glad/glad.h>
#include <vector>

// Shader
class Shader
{
private:
    GLuint m_program;
    std::vector<string> splitShaderSources(const char *glslCode);
public:
    Shader(const char *glslCode);
    void setFloat(const string &name, float value) const;
    void setVec4(const string &name, float v0, float v1, float v2, float v3) const;
    ~Shader();
    void use();
};