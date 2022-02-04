// Abstract away OS-implementation for window & input handling.
// Only one instance possible for the entire program life.

#pragma once

#include "core/defines.hpp"

enum class MouseButton {
    LEFT,
    MIDDLE,
    RIGHT
};

class Platform {
protected:
    Platform(Platform &other);

    void operator=(const Platform &);

    Platform(const string &title, int width, int height);

    ~Platform();

public:
    static Platform &getInstance(const string &title, int width, int height) {
        static Platform inst(title, width, height);
        return inst;
    }

    static Platform &getInstance() {
        return getInstance("APP", 640, 480);
    }

    bool processEvents();

    void swapBuffers();

    int getWidth();

    int getHeight();

    int getMouseX();

    int getMouseY();

    int getMouseWheel();

    bool isMouseDown(MouseButton button);

    bool isMouseUP(MouseButton button);

    bool isMousePressed(MouseButton button);

    bool isMouseReleased(MouseButton button);
};
