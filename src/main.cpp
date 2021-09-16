#include "core/defines.hpp"
#include "core/logger.hpp"
#include "platform/platform.hpp"
#include "render/render.hpp"

#include <glad/glad.h>



int main(int argc, char const *argv[])
{

    Platform &platform = Platform::getInstance("BasicBool", 640, 480);
    Renderer &renderer = Renderer::getInstance();
    while (platform.processEvents())
    {
        int width = platform.getWidth();
        int height = platform.getHeight();
        // TODO : Should be in the renderer
        glViewport(0, 0, width, height);

        
        renderer.clear(Color::WHITE);
        // TODO : Change implementation
        renderer.drawRectangle(width/6, height/4, 128, 256);
        // TODO : Change implementation
        renderer.drawRectangle(width/2, height/2, 64, 64);

        // TODO : create this
        // renderer.render()
        platform.swapBuffers();
    }
    return 0;
}
