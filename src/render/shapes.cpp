#include "shapes.hpp"


// QUAD
const float quadVertices[16] = {
        // pos  // uv
        -0.5, -0.5, 0, 0,  // top left
         0.5, -0.5, 1, 0,  // top right
         0.5,  0.5, 1, 1,  // bottom right
        -0.5,  0.5, 0, 1,  // bottom left
};
const unsigned int quadIndices[6] = {0, 1, 2, 0, 2, 3};



Shapes::Shapes() : m_quadVBO(quadVertices, sizeof(quadVertices)), m_quadEBO(quadIndices, sizeof(quadIndices)){
    // QUAD
    VertexBufferLayout rectLayout;
    rectLayout.push<float>(2); // pos
    rectLayout.push<float>(2); // uv
    m_quadVAO.addBuffer(m_quadVBO, rectLayout);
    m_quadVAO.addBuffer(m_quadEBO);
    m_quadVAO.unbind();
}


Shapes &Shapes::getInstance()
{
    static Shapes inst;
    return inst;
}

void Shapes::drawQuad()
{
    m_quadVAO.bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}