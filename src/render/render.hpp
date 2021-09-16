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

struct DrawCall {
    Color fillColor;
    Color strokeColor;
    float strokeWeight;
};

struct RectangleDrawCall : public DrawCall {
    float width;
    float height;
    float x;
    float y;
    RectangleDrawCall(Color fill, Color stroke, float weight, float x, float y, float width, float height){
        fillColor = fill;
        strokeColor = stroke;
        strokeWeight = weight;
        this->x = x;
        this->y = y;
        this-> width = width;
        this-> height = height;
    }
};

class Renderer
{
private:
    Color m_currentFillColor, m_currentStrokeColor;
    float m_currentStrokeWeight;
    std::vector<RectangleDrawCall> m_rectDrawCalls;

    void renderRectangles();

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


    // Primitives
    void drawRectangle(float x, float y, float width, float height);
    void drawCircle(float x, float y, float r);
    void drawLine(float x1, float y1, float x2, float y2);
    void drawText(float x, float y, string &text);

    void clear(const Color& color);
    void renderFrame();
};
