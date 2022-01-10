#include "backend.hpp"
#include "core/logger.hpp"
#include "core/math.hpp"
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// ---- SHADERS ----

std::vector<string> Shader::splitShaderSources(const string &glslCode)
{
    std::vector<string> shaderCodes;
    std::stringstream vertexCode;
    std::stringstream fragmentCode;
    std::stringstream geometryCode;
    std::istringstream f(glslCode);
    std::string line;
    enum CodeType
    {
        NONE,
        VEXRTEX,
        FRAGMENT,
        GEOMETRY
    };
    CodeType state = CodeType::NONE;
    bool geometryShader = false;
    while (std::getline(f, line))
    {
        // We will read a shader code !
        if (line.find("#SHADER") != string::npos)
        {
            // Vertex code
            if (line.find("VERTEX") != string::npos)
            {
                state = CodeType::VEXRTEX;
                continue;
            }
            else if (line.find("FRAGMENT") != string::npos)
            {
                state = CodeType::FRAGMENT;
                continue;
            }
            else if (line.find("GEOMETRY") != string::npos)
            {
                state = CodeType::GEOMETRY;
                continue;
            }
        }
        // Read shader code or skip
        switch (state)
        {
        case CodeType::VEXRTEX:
            vertexCode << line << '\n';
            break;

        case CodeType::FRAGMENT:
            fragmentCode << line << '\n';
            break;
        case CodeType::GEOMETRY:
            geometryShader = true;
            geometryCode << line << '\n';
            break;
        }
    }
    shaderCodes.push_back(vertexCode.str());
    shaderCodes.push_back(fragmentCode.str());
    if (geometryShader)
        shaderCodes.push_back(geometryCode.str());
    return shaderCodes;
}

Shader::Shader(const string &name)
{
    string path = "res/shaders/" + name + ".glsl";
    LOGDEBUG("Loading shader : {}", path);
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    string source = buffer.str();

    auto shaderCodes = splitShaderSources(source);

    const char *vertexCode = shaderCodes[0].c_str();
    const char *fragmentCode = shaderCodes[1].c_str();

    int success;
    char infoLog[512];
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LOGERROR("Vertex shader compilation failed :\n{}", infoLog);
    }

    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        LOGERROR("Fragment shader compilation failed :\n{}", infoLog);
    }

    m_program = glCreateProgram();
    glAttachShader(m_program, vertexShader);
    glAttachShader(m_program, fragmentShader);

    // We have a geometry shader
    GLuint geometryShader = 0;
    if (shaderCodes.size() == 3)
    {
        const char *geomatryCode = shaderCodes[2].c_str();
        GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geomatryCode, NULL);
        glCompileShader(geometryShader);
        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometryShader, 512, NULL, infoLog);
            LOGERROR("Geometry shader compilation failed :\n{}", infoLog);
        }
        glAttachShader(m_program, geometryShader);
    }

    glLinkProgram(m_program);
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_program, 512, NULL, infoLog);
        LOGERROR("Program shader linking failed :\n{}", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (shaderCodes.size() == 3)
        glDeleteShader(geometryShader);
}

Shader::~Shader()
{
    glDeleteProgram(m_program);
}
void Shader::use()
{
    glUseProgram(m_program);
}

void Shader::setFloat(const string &name, float value) const
{
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setInt(const string &name, int value) const
{
    glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setVec4(const string &name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(glGetUniformLocation(m_program, name.c_str()), v0, v1, v2, v3);
}
void Shader::setVec3(const string &name, float v0, float v1, float v2) const
{
    glUniform3f(glGetUniformLocation(m_program, name.c_str()), v0, v1, v2);
}
void Shader::setVec2(const string &name, float v0, float v1) const
{
    glUniform2f(glGetUniformLocation(m_program, name.c_str()), v0, v1);
}

void Shader::setVec4(const string &name, const vec4 &v) const
{
    glUniform4fv(glGetUniformLocation(m_program, name.c_str()), 1, (float *)&v);
}
void Shader::setVec2(const string &name, const vec2 &v) const
{
    glUniform2fv(glGetUniformLocation(m_program, name.c_str()), 1, (float *)&v);
}
void Shader::setVec3(const string &name, const vec3 &v) const
{
    glUniform3fv(glGetUniformLocation(m_program, name.c_str()), 1, (float *)&v);
}

void Shader::setMat4(const string &name, const mat4 &m) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, (float *)&m);
}
void Shader::setMat3(const string &name, const mat3 &m) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, (float *)&m);
}

// ---- VETEXBUFFER ----

VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_vbo);
}

void VertexBuffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
}
void VertexBuffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ---- ELEMENTBUFFER ----

ElementBuffer::ElementBuffer(const void *data, unsigned int size)
{
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

ElementBuffer::~ElementBuffer()
{
    glDeleteBuffers(1, &m_ebo);
}

void ElementBuffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
}
void ElementBuffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// ---- VERTEXBUFFERELEMENT ----
unsigned int VertexBufferElement::getTypeSize(unsigned int type)
{
    switch (type)
    {
    case GL_FLOAT:
        return sizeof(float);
    }
    LOGFATAL("VertexBufferElment::getTypeSize({}) -> unknow type", type);
    return 0;
}

// ---- VERTEXBUFFERLAYOUT ----

VertexBufferLayout::VertexBufferLayout() : m_stride(0) {}
inline const std::vector<VertexBufferElement> &VertexBufferLayout::getElements() const { return m_elements; }
inline unsigned int VertexBufferLayout::getStride() const { return m_stride; }

// ---- VERTEXARRAY ----

VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
}
VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &m_vao);
}
void VertexArray::bind() const
{
    glBindVertexArray(m_vao);
}
void VertexArray::unbind() const
{
    glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout)
{
    bind();
    vb.bind();
    const auto &elements = layout.getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto &element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void *)offset);
        offset += element.count * VertexBufferElement::getTypeSize(element.type);
    }
}

void VertexArray::addBuffer(const ElementBuffer &ebo)
{
    bind();
    ebo.bind();
}

// ---- TEXTURE ----

Texture::Texture(const string &path)
{
    LOGDEBUG("Loading image : {}", path);
    stbi_set_flip_vertically_on_load(false);
    int nbChannels;
    unsigned char *data = stbi_load(path.c_str(), &m_width, &m_height, &nbChannels, 0);

    if (data)
    {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (nbChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (nbChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        LOGERROR("Can't load image : {}", path);
    }
    stbi_image_free(data);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}

void Texture::bind(int unit)
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

int Texture::getWidth() { return m_width; }
int Texture::getHeight() { return m_height; }