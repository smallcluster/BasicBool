#pragma once

#include "core/defines.hpp"
#include <glad/glad.h>
#include <vector>

class Shader
{
private:
    GLuint m_program;
    std::vector<string> splitShaderSources(const char *glslCode);
public:
    Shader(const char *glslCode);
    void setFloat(const string &name, float value) const;
    void setInt(const string &name, int value) const;
    void setVec4(const string &name, float v0, float v1, float v2, float v3) const;
    void setVec2(const string &name, float v0, float v1) const;
    void setVec3(const string &name, float v0, float v1, float v2) const;
    ~Shader();
    void use();
};

class VertexBuffer
{
    private:
        GLuint m_vbo;
    public:
        VertexBuffer(const void *data, unsigned int size);
        ~VertexBuffer();
        void bind() const;
        void unbind() const;
};

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned int normalized;

    static unsigned int getTypeSize(unsigned int type);
};

class VertexBufferLayout
{
    private:
        std::vector<VertexBufferElement> m_elements;
        unsigned int m_stride;
    public:
        VertexBufferLayout();

        template<typename T>
        void push(unsigned int count)
        {
            LOGERROR("VertexBufferLayout::push() -> unknow type");
        }

        template<>
        void push<float>(unsigned int count)
        {
            m_elements.push_back({GL_FLOAT, count, GL_FALSE});
            m_stride += VertexBufferElement::getTypeSize(GL_FLOAT) * count;
        }

        inline const std::vector<VertexBufferElement>& getElements() const;
        inline unsigned int getStride() const;

};

class VertexArray
{
    private:
        GLuint m_vao;
    public:
        VertexArray();
        ~VertexArray();
        void addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
        void bind() const;
        void unbind() const;
};