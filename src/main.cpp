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
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

    // Shaders
    string basicSource = readShaderSource("res/shaders/basic.glsl");
    Shader basicShader(basicSource);

    string rectShadowSource = readShaderSource("res/shaders/rect_shadow.glsl");
    Shader rectShadowShader(rectShadowSource);

    while (platform.processEvents())
    {
        int width = platform.getWidth();
        int height = platform.getHeight();

        // Projection matrix
        mat4 pmat(vec4(2.0f/width, 0, 0, 0), vec4(0, -2.0f/height, 0, 0), vec4(0, 0, 1, 0), vec4(-1, 1, 0, 1));

        glViewport(0, 0, width, height);

        // TODO : make a simple GUI lib

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
        glDepthMask(GL_FALSE); // disable depth writing for lines
        glDrawArrays(GL_LINES, 0, 2*(nx+ny+2));
        glDepthMask(GL_TRUE);


        // rectangle shadows
        float rect[16] = {
            // pos    // uv
            width/2.0f-100, height/2.0f-100,     0, 0, // top left
            width/2.0f+100, height/2.0f-100,   1, 0, // top right
            width/2.0f+100, height/2.0f+100, 1, 1, // bottom right
            width/2.0f-100, height/2.0f+100,   0, 1, // bottom left

        };
        unsigned int rectIndices[6] = {0, 1, 2, 0, 2, 3};

        VertexArray rectVao;
        VertexBuffer rectVbo(rect, sizeof(rect));
        VertexBufferLayout rectLayout;
        rectLayout.push<float>(2); // pos
        rectLayout.push<float>(2); // uv
        rectVao.addBuffer(rectVbo, rectLayout);
        ElementBuffer rectEbo(rectIndices, sizeof(rectIndices));
        rectVao.addBuffer(rectEbo);

        rectShadowShader.use();
        rectShadowShader.setMat4("pmat", pmat);
        rectShadowShader.setVec4("color", vec4(vec3(0.0), 0.5));

        rectVao.bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        platform.swapBuffers();
    }
    return 0;
}
