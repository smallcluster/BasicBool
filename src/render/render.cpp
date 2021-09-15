#include "render.hpp"
#include "platform/platform.hpp"
#include "core/logger.hpp"
#include "shader.hpp"
#include <glad/glad.h>

// Some color constants
const Color Color::NOCOLOR = {0.0f, 0.0f, 0.0f, 0.0f};
const Color Color::BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const Color Color::WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const Color Color::RED = {1.0f, 0.0f, 0.0f, 1.0f};
const Color Color::GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const Color Color::BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
const Color Color::GREY = {0.5f, 0.5f, 0.5f, 1.0f};


void Renderer::clear(const Color& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::drawRectangle(float x, float y, float width, float height)
{
    // TODO : TESTING ONLY -> VERY BAD RESSOURCE MANAGEMENT
    Platform& p = Platform::getInstance();
    int w = p.getWidth();
    int h = p.getHeight();
    float coords[] = { 
        2.0f * (x / (float) w) - 1.0f, -2.0f * (y / (float) h) + 1.0f};
    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(coords), coords, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    const char *rectShaderSource = {
        #include "glsl/rectangle.h"
    };

    Shader rectShader(rectShaderSource);
    rectShader.use();
    rectShader.setFloat("width",  2.0f*width / (float) w);
    rectShader.setFloat("height", 2.0f*height / (float) h);
    rectShader.setVec4("fillColor", 1.0f, 0.0f, 0.0f, 1.0f);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

Renderer::~Renderer() {

}