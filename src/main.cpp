#include "core/defines.hpp"
#include "core/logger.hpp"
#include "core/math.hpp"
#include "platform/platform.hpp"
#include <glad/glad.h>
#include "render/backend.hpp"
#include "render/shapes.hpp"


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

    string nodeConnectorSource = readShaderSource("res/shaders/node_connector.glsl");
    Shader nodeConnectorShader(nodeConnectorSource);

    // Shapes
    Shapes &shapes = Shapes::getInstance();

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
        basicShader.setMat4("projection", pmat);
        basicShader.setMat4("transform", identity<4>());
        basicShader.setVec3("color", vec3(77.0f/255.0f));

        vao.bind();
        glDrawArrays(GL_LINES, 0, 2*(nx+ny+2));


        // ---- NODE TEST ---- //

        float cx = platform.getMouseX();
        float cy = platform.getMouseY();
        float w = 139;
        float headerHeight = 31;
        float bodyHeight = 57;
        float h = headerHeight+bodyHeight;
        float r = 8;
        float s = 16;

        // Shadow
        mat4 trans = identity<4>();
        trans = scale(trans, vec3(w+2*s, h+2*s, 1));
        trans = translate(trans, vec3(cx, cy, 0));
        nodeShadowShader.use();
        nodeShadowShader.setMat4("projection", pmat);
        nodeShadowShader.setMat4("transform", trans);
        nodeShadowShader.setFloat("width", w+2*s);
        nodeShadowShader.setFloat("height", h+2*s);
        nodeShadowShader.setFloat("smoothing", s);
        nodeShadowShader.setFloat("radius", r);
        shapes.drawQuad();

        // node
        trans = identity<4>();
        trans = scale(trans, vec3(w, h, 1));
        trans = translate(trans, vec3(cx, cy, 0));
        nodeShader.use();
        nodeShader.setMat4("projection", pmat);
        nodeShader.setMat4("transform", trans);
        nodeShader.setFloat("width", w);
        nodeShader.setFloat("height", h);
        nodeShader.setFloat("radius", r);
        nodeShader.setFloat("headerHeight", headerHeight);
        shapes.drawQuad();

        // connectors
        float dr = 16.0f;
        trans = identity<4>();
        trans = scale(trans, vec3(dr, dr, 1));
        trans = translate(trans, vec3(cx-w/2.0f+dr/2.0f+4, cy-h/2.0f+headerHeight+dr/2.0f+4, 0));
        nodeConnectorShader.use();
        nodeConnectorShader.setMat4("projection", pmat);
        nodeConnectorShader.setMat4("transform", trans);
        nodeConnectorShader.setFloat("width", dr);
        nodeConnectorShader.setFloat("height", dr);
        nodeConnectorShader.setFloat("radius", dr/2.0f);
        nodeConnectorShader.setVec3("insideColor", vec3(0));
        shapes.drawQuad();

        // End drawing
        platform.swapBuffers();
    }
    return 0;
}
