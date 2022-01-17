#include "core/defines.hpp"
#include "platform/platform.hpp"
#include "render/backend.hpp"
#include "render/shapes.hpp"
#include "core/math.hpp"
#include "render/text.hpp"
#include "node/nodes.hpp"
#include <chrono>
#include <thread>
#include <cmath>


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
    glEnable(GL_LINE_SMOOTH);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_STENCIL_TEST);

    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    Shader basicShader("basic");

    // Font
    Font font("roboto_regular_sdf");

    NodeManager NodeManager(font);

    vec2 orpos = vec2(platform.getWidth()/2.0f, platform.getHeight()/2.0f);

    Node* true1 = new TrueNode(orpos+vec2(-350, -100));
    Node* not1 = new NotNode(orpos+vec2(-200, -100));
    Node* not2 = new NotNode(orpos+vec2(-200, 100));
    Node* or1 = new OrNode(orpos);
    Node* not3 = new NotNode(orpos+vec2(200, 0));


    Link* true1Not1 = new Link(true1->getOutput("out"), not1->getInput("in"));

    Link* not1Or1 = new Link(not1->getOutput("out"), or1->getInput("in1"));
    Link* not2Or1 = new Link(not2->getOutput("out"), or1->getInput("in2"));
    Link* or1Not3 = new Link(or1->getOutput("out"), not3->getInput("in"));


    NodeManager.addNode(true1);
    NodeManager.addNode(not1);
    NodeManager.addNode(not2);
    NodeManager.addNode(or1);
    NodeManager.addNode(not3);

    NodeManager.addLink(true1Not1);
    NodeManager.addLink(not1Or1);
    NodeManager.addLink(not2Or1);
    NodeManager.addLink(or1Not3);


    // projection size
    vec2 viewOffset = vec2(0);
    float zoom = 1.0;

    double avgFps = 0;
    unsigned long long frame = 0;


    while (platform.processEvents())
    {
        auto startTime = std::chrono::high_resolution_clock::now();

        int width = platform.getWidth();
        int height = platform.getHeight();


        vec2 mouse = vec2(platform.getMouseX(), platform.getMouseY());

        int delta = platform.getMouseWheel();

        zoom += zoom*0.05f*delta;
        if(zoom < 0.1f)
            zoom = 0.1f;
        else if(zoom >= 10)
            zoom = 10;

        // View matrix
        mat4 view = scale(identity<4>(), vec3(zoom, zoom, 0));
        view = translate(view, vec3((1-zoom)*width/2.0f, (1-zoom)*height/2.0f, 1));

        // Projection matrix
        mat4 pmat(vec4( 2.0f / (float) width, 0, 0, 0), vec4(0, -2.0f / (float)height, 0, 0), vec4(0, 0, 1, 0), vec4(-1, 1, 0, 1));
        pmat = pmat;

        glViewport(0, 0, width, height);

        // Begin drawing

        // --- Background --- //
        float bgVal = 35.0f / 255.0f;
        glClearColor(bgVal, bgVal, bgVal, 1.0);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT);

        // --- Grid --- //
        // TODO : improve this
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
        basicShader.setMat4("view", identity<4>());
        basicShader.setVec3("color", vec3(77.0f / 255.0f));

        vao.bind();
        glDrawArrays(GL_LINES, 0, 2 * (nx + ny + 2));

        // ---- NODE TEST ---- //
        NodeManager.simulate();
        NodeManager.render(pmat, view);


        // wait time
        /*
        auto endTime = std::chrono::high_resolution_clock::now();
        long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(endTime-startTime).count();
        long long targetTime = 16666/2;
        long long waitTime = targetTime - elapsedTime;
        if(waitTime > 0){
            std::this_thread::sleep_for(std::chrono::microseconds(waitTime));
        }
        */
        auto finalTime = std::chrono::high_resolution_clock::now();
        long long finalElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(finalTime-startTime).count();
        int fps = (int) (1000000.0 / finalElapsedTime);
        string text = std::to_string(fps);
        font.text("fps : "+text, vec2(0), 20, vec3(1));

        double currentFps = (1000000.0 / finalElapsedTime);
        if(frame > 1){
            avgFps = avgFps + (long double) (currentFps-avgFps)/ (long double) frame;
        } else {
            avgFps = currentFps;
        }
        frame++;

        text = std::to_string((int) avgFps);
        font.text("avg fps : "+text, vec2(0, 20), 20, vec3(1));

        font.render(pmat);
        

        // End drawing
        platform.swapBuffers();

        
        
    }
    return 0;
}
