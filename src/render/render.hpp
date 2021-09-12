#pragma once

#include "core/defines.hpp"
#include <vector>

struct Color
{
    float r;
    float g;
    float b;
    float a;

    // Default colors
    const static Color NOCOLOR;
    const static Color BLACK;
    const static Color WHITE;
    const static Color RED;
    const static Color GREEN;
    const static Color BLUE;
    const static Color GREY;
};

class Renderer
{
private:
    Color currentFillColor, currentStrokeColor, backgroundColor;

protected:
    Renderer(Renderer &other);
    void operator=(const Renderer &);
    Renderer() : currentFillColor(Color::GREY), currentStrokeColor(Color::BLACK), backgroundColor(Color::WHITE) {}
    ~Renderer();

public:
    static Renderer &getInstance()
    {
        static Renderer inst;
        return inst;
    }

    // Inner color
    void fillColor(const Color &color);
    void noFill();

    // Stroke settings
    void strokeColor(const Color &color);
    void noStroke();
    void strokeWeight(float weight);

    // Primitives
    void drawRectangle(float x, float y, float width, float height);
    void drawCircle(float x, float y, float r);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawText(float x, float y, string &text);
};
