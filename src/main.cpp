#include "defines.hpp"

#include "core/logger.hpp"
#include "platform/platform.hpp"
#include <glad/glad.h>

int main(int argc, char const *argv[])
{

    Platform& platform = Platform::getInstance("BasicBool", 640, 480);

    float vertices[] = {
         0.0f,  0.5f, 0.0f, 0.0f, 1.0f, // top
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom left
         0.5f, -0.5f, 1.0f, 0.0f, 0.0f  // bottom right
    };
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, NULL, 5*sizeof(float), (void*)0); // Pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, NULL, 5*sizeof(float), (void*)(2*sizeof(float))); // color
    glEnableVertexAttribArray(1);

    const char * vertexCode = 
    "#version 330 core\n"
    "layout(location = 0) in vec2 aPos;\n"
    "layout(location = 1) in vec3 aColor;\n"
    "out vec3 color;\n"
    "void main(){gl_Position = vec4(aPos, 0.0, 1.0); color = aColor;}\n";

    const char * fragmentCode = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 color;\n"
    "void main(){FragColor = vec4(color, 1.0);}\n";

    int success;
    char infoLog[512];
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vertexCode, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        LOGERROR("Vertex shader compilation failed :\n{}", infoLog);
    }
    
    glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        LOGERROR("Fragment shader compilation failed :\n{}", infoLog);
    }

    GLuint programShader = glCreateProgram();
    glAttachShader(programShader, vertexShader);
    glAttachShader(programShader, fragmentShader);
    glLinkProgram(programShader);
    glGetProgramiv(programShader, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(programShader, 512, NULL, infoLog);
        LOGERROR("Program shader linking failed :\n{}", infoLog);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (platform.processEvents())
    {
        glViewport(0, 0, platform.getWidth(), platform.getHeight());
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(vao);
        glUseProgram(programShader);
        glDrawArrays(GL_TRIANGLES, 0, 3);


        platform.swapBuffers();
    }
    return 0;
}
