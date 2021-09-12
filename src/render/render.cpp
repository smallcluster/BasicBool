#include "render.hpp"
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