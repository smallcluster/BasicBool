#include "core/defines.hpp"
#include "platform/platform.hpp"
#include "render/backend.hpp"
#include "render/shapes.hpp"
#include "core/math.hpp"
#include "render/text.hpp"


int main(int argc, char const *argv[])
{
    // Math vector & matrix packing check
    static_assert(sizeof(vec2) == sizeof(GLfloat) * 2, "Can't pack vec2.");
    static_assert(sizeof(vec3) == sizeof(GLfloat) * 3, "Can't pack vec3.");
    static_assert(sizeof(vec4) == sizeof(GLfloat) * 4, "Can't pack vec4.");
    static_assert(sizeof(mat4) == sizeof(GLfloat) * 16, "Can't pack mat4.");
    static_assert(sizeof(mat3) == sizeof(GLfloat) * 9, "Can't pack mat3.");

    Platform &platform = Platform::getInstance("BasicBool", 1280, 720);

    // Opengl Setup
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    Shader basicShader("basic");
    Shader nodeShader("node");
    Shader nodeShadowShader("node_shadow");
    Shader nodeConnectorShader("node_connector");
    Shader basicTextureShader("basic_texture");
    Shader textShader("text_sdf");

    // Font
    Font font("roboto_regular_sdf");

    // Shapes
    Shapes &shapes = Shapes::getInstance();


    // projection size
    float size = 1.0;

    while (platform.processEvents())
    {
        int width = platform.getWidth();
        int height = platform.getHeight();

        vec2 mouse = vec2(platform.getMouseX(), platform.getMouseY());
        int delta = platform.getMouseWheel();
        size += delta*0.1f;

        // Projection matrix
        mat4 pmat(vec4(2.0f / width*size, 0, 0, 0), vec4(0, -2.0f / height*size, 0, 0), vec4(0, 0, 1, 0), vec4(-1, 1, 0, 1));

        glViewport(0, 0, width, height);

        // Begin drawing

        // --- Background --- //
        float bgVal = 35.0f / 255.0f;
        glClearColor(bgVal, bgVal, bgVal, 1.0);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- Grid --- //
        int nx = width / 16;
        int ny = height / 16;
        std::vector<vec2> data;
        data.reserve(nx + ny);
        for (int i = 0; i <= nx; ++i)
        {
            data.push_back(vec2(16.0f * i, 0));
            data.push_back(vec2(16.0f * i, height));
        }
        for (int i = 0; i <= ny; ++i)
        {
            data.push_back(vec2(0, 16.0f * i));
            data.push_back(vec2(width, 16.0f * i));
        }
        VertexArray vao;
        VertexBuffer vbo(&data[0], 4 * (nx + ny + 2) * sizeof(float));
        VertexBufferLayout layout;
        layout.push<float>(2);
        vao.addBuffer(vbo, layout);

        basicShader.use();
        basicShader.setMat4("projection", pmat);
        basicShader.setMat4("transform", identity<4>());
        basicShader.setVec3("color", vec3(77.0f / 255.0f));

        vao.bind();
        glDrawArrays(GL_LINES, 0, 2 * (nx + ny + 2));

        

        // ---- NODE TEST ---- //

        string headerText = "NOT node test BIG 00";
        float headerTextSize = 18;

        float cx = platform.getMouseX();
        float cy = platform.getMouseY();
        float r = 8;
        float s = 16;
        float w = font.getWidth(headerText, headerTextSize) + 2 * r;
        float headerHeight = font.getHeight(headerText, headerTextSize) + r;
        float bodyHeight = 57;
        float h = headerHeight + bodyHeight;

        // Shadow
        mat4 trans = identity<4>();
        trans = scale(trans, vec3(w + 2 * s, h + 2 * s, 1));
        trans = translate(trans, vec3(cx, cy, 0));
        nodeShadowShader.use();
        nodeShadowShader.setMat4("projection", pmat);
        nodeShadowShader.setMat4("transform", trans);
        nodeShadowShader.setFloat("width", w + 2 * s);
        nodeShadowShader.setFloat("height", h + 2 * s);
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
        trans = translate(trans, vec3(cx - w / 2.0f + dr / 2.0f + 4, cy - h / 2.0f + headerHeight + dr / 2.0f + 4, 0));
        nodeConnectorShader.use();
        nodeConnectorShader.setMat4("projection", pmat);
        nodeConnectorShader.setMat4("transform", trans);
        nodeConnectorShader.setFloat("width", dr);
        nodeConnectorShader.setFloat("height", dr);
        nodeConnectorShader.setFloat("radius", dr / 2.0f);
        nodeConnectorShader.setVec3("insideColor", vec3(0));
        shapes.drawQuad();

        // Text rendering
        font.text(headerText, vec2(cx - w / 2 + 8, cy - h / 2 + 8), headerTextSize, vec3(1));

        font.text("In", vec2(cx - w / 2.0f + dr / 2.0f + 16, cy - h / 2.0f + headerHeight + dr / 2.0f), 12, vec3(1));

        textShader.use();
        textShader.setMat4("projection", pmat);
        font.render();

        // End drawing
        platform.swapBuffers();
    }
    return 0;
}
