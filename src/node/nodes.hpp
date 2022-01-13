#pragma once
#include "core/defines.hpp"
#include "core/math.hpp"

class Node {

    private:
        vec2 m_dim;
        float m_headerHeight;
        string m_name;

        void updateSize();

    public:
        vec2 getDim();
        float getHeaderHeight();

        string setName(const string name);
        void setDim(vec2 dim);
};