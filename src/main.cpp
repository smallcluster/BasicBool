#include "core/defines.hpp"
#include "core/logger.hpp"
#include "platform/platform.hpp"
#include <glad/glad.h>

#include "render/render.hpp"
#include "render/shader.hpp"

int main(int argc, char const *argv[])
{

    Platform &platform = Platform::getInstance("BasicBool", 640, 480);
    Renderer &renderer = Renderer::getInstance();

    // TODO : remove demo

    // float vertices[] = {
    //     0.0f, 0.5f, 0.0f, 0.0f, 1.0f,   // top
    //     -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom left
    //     0.5f, -0.5f, 1.0f, 0.0f, 0.0f   // bottom right
    // };
    // GLuint vao, vbo;
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);
    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0); // Pos
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(2 * sizeof(float))); // color
    // glEnableVertexAttribArray(1);

    // // Test : static included shader
    // const char *basicTriangleCode = {
    //     #include "render/glsl/test_triangle.h"
    // };
    // Shader testShader(basicTriangleCode);

    while (platform.processEvents())
    {
        // glViewport(0, 0, platform.getWidth(), platform.getHeight());
        // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glBindVertexArray(vao);
        // testShader.use();
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        int width = platform.getWidth();
        int height = platform.getHeight();

        // TODO : implement the necessary things
        renderer.clear(Color::WHITE);
        renderer.drawRectangle(0, height/2, 64, 64);
        

        platform.swapBuffers();
    }
    return 0;
}
