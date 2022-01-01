#pragma once
#include "backend.hpp"

class Shapes
{
protected:
    Shapes(Shapes &other);
    void operator=(const Shapes &);
    Shapes();

private:
    // WARNING :
    // VBO & EBO need to be stored, thanks to a bug with AMD drivers not letting OpenGL manage
    // the memory by it self.

    // QUAD
    VertexArray m_quadVAO;
    VertexBuffer m_quadVBO;
    ElementBuffer m_quadEBO;

public:
    static Shapes &getInstance();
    void drawQuad();
};
