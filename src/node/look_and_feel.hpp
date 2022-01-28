#pragma once
#include "render/text.hpp"

struct NodeStyle
{
    vec3 nodeSelectedColor = vec3(1,0.5,0);
    vec3 trueColor = vec3(0, 1, 0);
    vec3 falseColor = vec3(1, 0, 0);
    vec3 headerColor = vec3(0, 0, 1);
    vec3 bodyColor = vec3(0.2);
    vec3 headerTextColor = vec3(1);
    vec3 connectorTextColor = vec3(1);
    float headerTextSize = 20;
    float connectorTextSize = 16;
    float connectorRadius = 8;
    vec2 connectorMargin = vec2(4, 4);
    float inOutDist = 32;
    float nodeRadius = 8;
    float shadowSize = 16;
    float textOutletMargin = 4;
    Font &font;
    NodeStyle(Font &font) : font(font) {};
    NodeStyle() : font(Font::getDefault()) {};
};
