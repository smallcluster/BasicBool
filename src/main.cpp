#include "defines.hpp"

#include "core/logger.hpp"
#include "platform/platform.hpp"

int main(int argc, char const *argv[])
{
    Platform& platform = Platform::getInstance("BasicBool", 1280, 720);

    while (platform.processEvents())
    {
        platform.swapBuffers();
    }
    return 0;
}
