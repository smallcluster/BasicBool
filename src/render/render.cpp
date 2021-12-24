#include "render.hpp"
#include "platform/platform.hpp"
#include "core/logger.hpp"
#include <glad/glad.h>

// Shader sources
const char *rectShaderSource = {
#include "glsl/rectangle.h"
};
const char *ellipseShaderSource = {
#include "glsl/ellipse.h"
};

// Some color constants
const Color Color::NOCOLOR = {0.0f, 0.0f, 0.0f, 0.0f};
const Color Color::BLACK = {0.0f, 0.0f, 0.0f, 1.0f};
const Color Color::WHITE = {1.0f, 1.0f, 1.0f, 1.0f};
const Color Color::RED = {1.0f, 0.0f, 0.0f, 1.0f};
const Color Color::GREEN = {0.0f, 1.0f, 0.0f, 1.0f};
const Color Color::BLUE = {0.0f, 0.0f, 1.0f, 1.0f};
const Color Color::GREY = {0.5f, 0.5f, 0.5f, 1.0f};

Renderer::Renderer() : m_currentFillColor(Color::GREY), m_currentStrokeColor(Color::BLACK), m_currentStrokeWeight(1.0f) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0x00);
}

Renderer::~Renderer() {}

void Renderer::clear(const Color &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

// TODO : use stencil buffer for stroke
void Renderer::drawRectangle(float x, float y, float width, float height)
{
    Platform &p = Platform::getInstance();
    int w = p.getWidth();
    int h = p.getHeight();
    float coords[] = {2.0f * (x / (float)w) - 1.0f, -2.0f * (y / (float)h) + 1.0f};
    VertexArray vao;
    VertexBuffer vbo(coords, 2 * sizeof(float));
    VertexBufferLayout layout;
    layout.push<float>(2);
    vao.addBuffer(vbo, layout);
    Shader rectShader(rectShaderSource);
    rectShader.use();
    rectShader.setFloat("width", 2.0f * width / (float)w);
    rectShader.setFloat("height", 2.0f * height / (float)h);
    rectShader.setVec2("sw", m_currentStrokeWeight / width, m_currentStrokeWeight / height);
    rectShader.setVec4("fillColor", m_currentFillColor.r, m_currentFillColor.g, m_currentFillColor.b, m_currentFillColor.a);
    rectShader.setVec4("strokeColor", m_currentStrokeColor.r, m_currentStrokeColor.g, m_currentStrokeColor.b, m_currentStrokeColor.a);
    vao.bind();
    glDrawArrays(GL_POINTS, 0, 1);
    vao.unbind();
}

// TODO : use stencil buffer for stroke
void Renderer::drawEllipse(float x, float y, float width, float height)
{
    Platform &p = Platform::getInstance();
    int w = p.getWidth();
    int h = p.getHeight();
    float coords[] = {2.0f * (x / (float)w) - 1.0f, -2.0f * (y / (float)h) + 1.0f};
    VertexArray vao;
    VertexBuffer vbo(coords, 2 * sizeof(float));
    VertexBufferLayout layout;
    layout.push<float>(2);
    vao.addBuffer(vbo, layout);
    Shader rectShader(ellipseShaderSource);
    rectShader.use();
    rectShader.setFloat("width", 2.0f * width / (float)w);
    rectShader.setFloat("height", 2.0f * height / (float)h);
    rectShader.setVec2("sw", m_currentStrokeWeight / width, m_currentStrokeWeight / height);
    rectShader.setVec4("fillColor", m_currentFillColor.r, m_currentFillColor.g, m_currentFillColor.b, m_currentFillColor.a);
    rectShader.setVec4("strokeColor", m_currentStrokeColor.r, m_currentStrokeColor.g, m_currentStrokeColor.b, m_currentStrokeColor.a);
    vao.bind();
    glDrawArrays(GL_POINTS, 0, 1);
    vao.unbind();
}

void Renderer::fillColor(Color color) { m_currentFillColor = color; }
void Renderer::strokeColor(Color color) { m_currentStrokeColor = color; }
void Renderer::strokeWeight(float w) { m_currentStrokeWeight = w; }