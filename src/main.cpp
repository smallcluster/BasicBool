#include "core/defines.hpp"
#include "core/logger.hpp"
#include "platform/platform.hpp"
#include "render/render.hpp"

#include <glad/glad.h>



int main(int argc, char const *argv[])
{

    Platform &platform = Platform::getInstance("BasicBool", 1280, 720);
    Renderer &renderer = Renderer::getInstance();
    
    while (platform.processEvents())
    {
        int width = platform.getWidth();
        int height = platform.getHeight();

        // TODO : Should be in the renderer
        glViewport(0, 0, width, height);

        renderer.clear(Color::WHITE);
        renderer.strokeWeight(4.0f);
        renderer.fillColor(Color::RED);
        renderer.strokeColor(Color::BLACK);
        renderer.drawRectangle(0, 0, 256, 256);
        renderer.drawEllipse(256, 256, 256, 256);
        renderer.fillColor(Color::GREEN);
        renderer.strokeColor(Color::BLACK);
        renderer.drawEllipse(256+64, 0, 128, 256);


        // Should be in the renderer
        platform.swapBuffers();
    }
    return 0;
}
