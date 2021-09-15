#include "shader.hpp"
#include "core/logger.hpp"

std::vector<string> Shader::splitShaderSources(const char *glslCode)
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
        GEOMETRY // TODO : handle this case
    };
    CodeType state = CodeType::NONE;
    while (std::getline(f, line))
    {
        // We will read a shader code !
        if (line.find("#SHADER") != string::npos)
        {
            // Vertex code
            if (line.find("VERTEX") != string::npos){
                state = CodeType::VEXRTEX;
                continue;
            }
            else if (line.find("FRAGMENT") != string::npos){
                state = CodeType::FRAGMENT;
                continue;
            } else if(line.find("GEOMETRY") != string::npos){
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
            geometryCode << line << '\n';
            break;
        }
    }
    shaderCodes.push_back(vertexCode.str());
    shaderCodes.push_back(fragmentCode.str());
    shaderCodes.push_back(geometryCode.str());
    return shaderCodes;
}

Shader::Shader(const char *glslCode)
{
    auto shaderCodes = splitShaderSources(glslCode);
    const char* vertexCode = shaderCodes[0].c_str();
    const char* fragmentCode = shaderCodes[1].c_str();
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
    if(shaderCodes.size() == 3){
        const char* geomatryCode = shaderCodes[2].c_str();
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
    if(shaderCodes.size() == 3)
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

void Shader::setFloat(const string &name, float value) const {
    glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
}

void Shader::setVec4(const string &name, float v0, float v1, float v2, float v3) const {
    glUniform4f(glGetUniformLocation(m_program, name.c_str()), v0, v1, v2, v3);
}
