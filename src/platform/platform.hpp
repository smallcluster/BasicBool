// Abstract away OS-implementation for window & input handling.
// Only one instance possible for the entire program life.

#pragma once

#include "defines.hpp"

class Platform
{ 
protected:
    Platform(Platform &other);
    void operator=(const Platform &);
    Platform(const string &title, int width, int height);
    ~Platform();

public:
    static Platform &getInstance(const string &title, int width, int height)
    {
        static Platform inst(title, width, height);
        return inst;
    }
    static Platform &getInstance()
    {
        return getInstance("APP", 1280, 720);
    }

    bool processEvents();
    void swapBuffers();
    static void fatalError(const string &message);
};
