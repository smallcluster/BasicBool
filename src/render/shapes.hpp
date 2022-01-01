#pragma once
#include "backend.hpp"

class Shapes
{
protected:
    Shapes(Shapes &other);
    void operator=(const Shapes &);
    Shapes();

private:
    // QUAD
    // WARNING :
    // VBO & EBO are stored du to a bug on AMD drivers not letting OpenGL manage
    // the memory by it self.
    VertexArray m_quadVAO;
    VertexBuffer m_quadVBO;
    ElementBuffer m_quadEBO;

public:
    static Shapes &getInstance();
    void drawQuad();
};
