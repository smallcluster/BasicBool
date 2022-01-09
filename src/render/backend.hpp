#pragma once
#include "core/defines.hpp"
#include "core/logger.hpp"
#include "core/math.hpp"
#include <glad/glad.h>
#include <vector>


class Shader
{
private:
    GLuint m_program;
    std::vector<string> splitShaderSources(const string &glslCode);
public:
    Shader(const string &glslCode);
    void setFloat(const string &name, float value) const;

    void setInt(const string &name, int value) const;
    void setVec4(const string &name, float v0, float v1, float v2, float v3) const;
    void setVec2(const string &name, float v0, float v1) const;
    void setVec3(const string &name, float v0, float v1, float v2) const;

    void setVec4(const string &name, const vec4 &v) const;
    void setVec2(const string &name, const vec2 &v) const;
    void setVec3(const string &name, const vec3 &v) const;
    void setMat4(const string &name, const mat4 &m) const;
    void setMat3(const string &name, const mat3 &m) const;


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

class ElementBuffer
{
    private:
        GLuint m_ebo;
    public:
        ElementBuffer(const void *data, unsigned int size);
        ~ElementBuffer();
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
        void addBuffer(const ElementBuffer& ebo);
        void bind() const;
        void unbind() const;
};

// TODO : implementation
class Texture{
    private:
        unsigned int m_texture;
        int m_unit = 0;
        int m_width;
        int m_height;
    public:
        Texture(const string &path);
        ~Texture();
        void bind(int unit);
        void unbind();
};

string readShaderSource(const string &path);