#include "core/defines.hpp"
#include "core/logger.hpp"
#include "core/math.hpp"
#include "platform/platform.hpp"
#include <glad/glad.h>
#include "render/backend.hpp"


int main(int argc, char const *argv[])
{
    // Math vector & matrix packing check
    static_assert(sizeof(vec2) == sizeof(GLfloat) * 2,  "Can't pack vec2.");
    static_assert(sizeof(vec3) == sizeof(GLfloat) * 3,  "Can't pack vec3.");
    static_assert(sizeof(vec4) == sizeof(GLfloat) * 4,  "Can't pack vec4.");
    static_assert(sizeof(mat4) == sizeof(GLfloat) * 16, "Can't pack mat4.");
    static_assert(sizeof(mat3) == sizeof(GLfloat) * 9,  "Can't pack mat3.");

    Platform &platform = Platform::getInstance("BasicBool", 1280, 720);

    // Opengl Setup
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Shaders
    string basicSource = readShaderSource("res/shaders/basic.glsl");
    Shader basicShader(basicSource);

    string nodeSource = readShaderSource("res/shaders/node.glsl");
    Shader nodeShader(nodeSource);

    string nodeShadowSource = readShaderSource("res/shaders/node_shadow.glsl");
    Shader nodeShadowShader(nodeShadowSource);

    while (platform.processEvents())
    {
        int width = platform.getWidth();
        int height = platform.getHeight();

        // Projection matrix
        mat4 pmat(vec4(2.0f/width, 0, 0, 0), vec4(0, -2.0f/height, 0, 0), vec4(0, 0, 1, 0), vec4(-1, 1, 0, 1));

        glViewport(0, 0, width, height);

        // Begin drawing


        // --- Background --- //
        float bgVal = 35.0f/255.0f;
        glClearColor(bgVal, bgVal, bgVal, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        
        // --- Grid --- //
        int nx = width / 16;
        int ny = height / 16;
        std::vector<vec2> data;
        data.reserve(nx+ny);
        for(int i=0; i <= nx; ++i){
            data.push_back(vec2(16.0f*i, 0));
            data.push_back(vec2(16.0f*i, height));
        }
        for(int i=0; i <= ny; ++i){
            data.push_back(vec2(0, 16.0f*i));
            data.push_back(vec2(width, 16.0f*i));
        }
        VertexArray vao;
        VertexBuffer vbo(&data[0], 4*(nx+ny+2)*sizeof(float));
        VertexBufferLayout layout;
        layout.push<float>(2);
        vao.addBuffer(vbo, layout);

        basicShader.use();
        basicShader.setMat4("pmat", pmat);
        basicShader.setVec3("color", vec3(77.0f/255.0f));

        vao.bind();
        glDrawArrays(GL_LINES, 0, 2*(nx+ny+2));


        // ---- NODE TEST ---- //

        float cx = width/2.0f;
        float cy = height/2.0f;
        float w = 139;
        float headerHeight = 31;
        float bodyHeight = 57;
        float h = headerHeight+bodyHeight;
        float r = 8;
        float s = 16;
        unsigned int rectIndices[6] = {0, 1, 2, 0, 2, 3};

        // Shadow
        float rect[16] = {
            // pos    // uv
            cx-w/2.0f-s, cy-h/2.0f-s,     0, 0, // top left
            cx+w/2.0f+s, cy-h/2.0f-s,     1, 0, // top right
            cx+w/2.0f+s, cy+h/2.0f+s,     1, 1, // bottom right
            cx-w/2.0f-s, cy+h/2.0f+s,     0, 1, // bottom left
        };
        VertexArray rectVao;
        VertexBuffer rectVbo(rect, sizeof(rect));
        VertexBufferLayout rectLayout;
        rectLayout.push<float>(2); // pos
        rectLayout.push<float>(2); // uv
        rectVao.addBuffer(rectVbo, rectLayout);
        ElementBuffer rectEbo(rectIndices, sizeof(rectIndices));
        rectVao.addBuffer(rectEbo);

        nodeShadowShader.use();
        nodeShadowShader.setMat4("pmat", pmat);
        nodeShadowShader.setFloat("width", w+2*s);
        nodeShadowShader.setFloat("height", h+2*s);
        nodeShadowShader.setFloat("smoothing", s);
        nodeShadowShader.setFloat("radius", r);
        rectVao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        // node
        float rectNode[16] = {
            // pos    // uv
            cx-w/2.0f, cy-h/2.0f,     0, 0, // top left
            cx+w/2.0f, cy-h/2.0f,     1, 0, // top right
            cx+w/2.0f, cy+h/2.0f,     1, 1, // bottom right
            cx-w/2.0f, cy+h/2.0f,     0, 1, // bottom left
        };
        VertexArray rectNodeVao;
        VertexBuffer rectNodeVbo(rectNode, sizeof(rectNode));
        VertexBufferLayout rectNodeLayout;
        rectNodeLayout.push<float>(2); // pos
        rectNodeLayout.push<float>(2); // uv
        rectNodeVao.addBuffer(rectNodeVbo, rectNodeLayout);
        ElementBuffer rectNodeEbo(rectIndices, sizeof(rectIndices));
        rectNodeVao.addBuffer(rectNodeEbo);

        nodeShader.use();
        nodeShader.setMat4("pmat", pmat);
        nodeShader.setFloat("width", w);
        nodeShader.setFloat("height", h);
        nodeShader.setFloat("radius", r);
        nodeShader.setFloat("headerHeight", headerHeight);
        rectNodeVao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // End drawing
        platform.swapBuffers();
    }
    return 0;
}
