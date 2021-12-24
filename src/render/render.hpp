#pragma once

#include "core/defines.hpp"
#include "backend.hpp"
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
    Color m_currentFillColor, m_currentStrokeColor;
    float m_currentStrokeWeight;

protected:
    Renderer(Renderer &other);
    void operator=(const Renderer &);
    Renderer();
    ~Renderer();

public:
    static Renderer &getInstance()
    {
        static Renderer inst;
        return inst;
    }

    void fillColor(Color color);
    void strokeColor(Color color);
    void strokeWeight(float w);


    void drawRectangle(float x, float y, float width, float height);
    void drawEllipse(float x, float y, float width, float height);

    // TODO : implement this
    void drawLine(float x1, float y1, float x2, float y2);
    void drawText(float x, float y, string &text);
    void noStroke();
    void noFill();
    void textSize(float pt);
    float getTextHeight(string &text);
    float getTextWidth(string &text);

    void clear(const Color& color);
};
