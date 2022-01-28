#include "core/defines.hpp"
#include "platform/platform.hpp"
#include "render/backend.hpp"
#include "render/shapes.hpp"
#include "core/math.hpp"
#include "render/text.hpp"
#include "node/node_system.hpp"
#include "node/nodes.hpp"
#include <chrono>
#include <thread>
#include <cmath>
#include <optional>
#include <thread>

using namespace std::chrono_literals;

// ---------------------------------------------------------------------

//-----------------------------------------------------------------------
vec2 viewOffset = vec2(0);
float zoom = 1.0f;
vec2 oldMouse = vec2(0);
vec2 mouse = vec2(0);
bool viewPanning = false;

mat4 getViewMatrix()
{
    mat4 view = scale(identity<4>(), vec3(zoom, zoom, 0));
    vec3 viewTranslate = vec3((1 - zoom) / 2.0f, (1 - zoom) / 2.0f, 0) + vec3(viewOffset, 0);
    view = translate(view, viewTranslate);
    return view;
}
mat4 getInvViewMatrix()
{
    mat4 invView = scale(identity<4>(), vec3(1.0f / zoom, 1.0f / zoom, 0));
    vec3 viewTranslate = vec3((1 - zoom) / 2.0f, (1 - zoom) / 2.0f, 0) + vec3(viewOffset, 0);
    invView = translate(invView, -viewTranslate / zoom);
    return invView;
}

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
    Font &font = Font::getDefault();

    NodeManager NodeManager;

    unsigned int tickPerSec = 1;
    auto simStartTime = std::chrono::system_clock::now();
    auto tickTime = 1000000000ns / tickPerSec;
    bool beginUpdateDone = false;
    bool boxSelection = false;
    vec2 boxSelectionStart = vec2(0);

    bool unlimitedTickTime = false;

    vec2 orpos = vec2(platform.getWidth() / 2.0f, platform.getHeight() / 2.0f);

    Node *true1 = new TrueNode(orpos + vec2(-350, -100));
    Node *not1 = new NotNode(orpos + vec2(-200, -100));
    Node *not2 = new NotNode(orpos + vec2(-200, 100));
    Node *or1 = new OrNode(orpos);
    Node *not3 = new NotNode(orpos + vec2(200, 0));

    NodeManager.addNode(true1);
    NodeManager.addNode(not1);
    NodeManager.addNode(not2);
    NodeManager.addNode(or1);
    NodeManager.addNode(not3);

    /*
    for (int i = 0; i < 10000; i++)
    {

        Node *n1 = new TrueNode(25.0f * vec2(std::rand() % platform.getWidth(), std::rand() % platform.getHeight()));
        Node *n2 = new NotNode(25.0f * vec2(std::rand() % platform.getWidth(), std::rand() % platform.getHeight()));
        Link *n1n2 = new Link(n1->getOutput("out"), n2->getInput("in"));
        NodeManager.addLink(n1n2);
        NodeManager.addNode(n1);
        NodeManager.addNode(n2);

        Node *n3 = new NotNode(25.0f * vec2(std::rand() % platform.getWidth(), std::rand() % platform.getHeight()));
        Node *n4 = new NotNode(25.0f * vec2(std::rand() % platform.getWidth(), std::rand() % platform.getHeight()));
        Link *n3n4 = new Link(n3->getOutput("out"), n4->getInput("in"));
        Link *n1n3 = new Link(n1->getOutput("out"), n3->getInput("in"));
        NodeManager.addLink(n3n4);
        NodeManager.addLink(n1n3);
        NodeManager.addNode(n3);
        NodeManager.addNode(n4);
    }*/

    // projection size
    double avgFps = 0;
    unsigned long long frame = 0;
    std::optional<Node *> grabNode = {};
    vec2 grabOffset = vec2(0);
    std::optional<Connector *> startConnector = {};

    while (platform.processEvents())
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        int width = platform.getWidth();
        int height = platform.getHeight();
        // update mouse pos
        oldMouse = mouse;
        vec2 oldWorldMouse = (getInvViewMatrix() * vec4(mouse, 0, 1)).xy;

        mouse = vec2(platform.getMouseX(), platform.getMouseY());
        // move view
        if (viewPanning)
        {
            viewOffset = viewOffset + mouse - oldMouse;
        }
        int delta = platform.getMouseWheel();
        // update zoom
        if (delta != 0)
        {
            vec2 tempMouse = (getInvViewMatrix() * vec4(mouse, 0, 1)).xy;
            zoom += zoom * 0.05f * delta;
            if (zoom < 0.1f)
                zoom = 0.1f;
            else if (zoom >= 10)
                zoom = 10;
            vec2 newWorldMouse = (getInvViewMatrix() * vec4(mouse, 0, 1)).xy;
            viewOffset = viewOffset + (newWorldMouse - tempMouse) * zoom;
        }

        mat4 view = getViewMatrix();
        mat4 invView = getInvViewMatrix();
        vec2 worldMouse = (invView * vec4(mouse, 0, 1)).xy;

        // Projection matrix
        mat4 pmat(vec4(2.0f / (float)width, 0, 0, 0), vec4(0, -2.0f / (float)height, 0, 0), vec4(0, 0, 1, 0), vec4(-1, 1, 0, 1));

        if (platform.isMousePressed(MouseButton::LEFT))
        {
            startConnector = NodeManager.getConnectorAt(worldMouse);

            if (!startConnector)
            {
                grabNode = NodeManager.getNodeAt(worldMouse);
                if (grabNode)
                {
                    grabOffset = grabNode.value()->pos - worldMouse;
                    if (!NodeManager.nodeIsSelected(grabNode.value()))
                    {
                        NodeManager.deselectAll();
                    }
                    NodeManager.selectNode(grabNode.value());
                }
                else
                {
                    NodeManager.deselectAll();
                    boxSelection = true;
                    boxSelectionStart = worldMouse;
                }
            }
        }
        else if (platform.isMouseReleased(MouseButton::LEFT))
        {
            grabNode = {};
            if (startConnector)
            {
                auto endConnector = NodeManager.getConnectorAt(worldMouse);
                if (endConnector)
                    NodeManager.connect(startConnector.value(), endConnector.value());
                startConnector = {};
            }
            if (boxSelection)
            {
                boxSelection = false;
                vec2 boxSelectionEnd = worldMouse;
                vec2 boxStart = vec2(std::min(boxSelectionStart.x, boxSelectionEnd.x), std::min(boxSelectionStart.y, boxSelectionEnd.y));
                vec2 boxEnd = vec2(std::max(boxSelectionStart.x, boxSelectionEnd.x), std::max(boxSelectionStart.y, boxSelectionEnd.y));
                NodeManager.boxSelect(boxStart, boxEnd);
            }
        }

        if (platform.isMousePressed(MouseButton::RIGHT))
        {
            auto c = NodeManager.getConnectorAt(worldMouse);
            if (c)
            {
                NodeManager.disconnectAll(c.value());
            }
            else
            {
                auto node = NodeManager.getNodeAt(worldMouse);
                if (node)
                {
                    NodeManager.selectNode(node.value());
                    NodeManager.removeSelected();
                }




            }
        }

        if (platform.isMousePressed(MouseButton::MIDDLE))
        {
            viewPanning = true;
        }
        if (platform.isMouseReleased(MouseButton::MIDDLE))
        {
            viewPanning = false;
        }

        if (grabNode)
        {
            //grabNode.value()->pos = worldMouse + grabOffset;
            NodeManager.moveSelectedNodes(worldMouse - oldWorldMouse);
        }

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

        if (startConnector)
        {
            NodeManager.drawTempLink(startConnector.value(), worldMouse, pmat, view);
        }

        // ---- simulation ----

        // start cycle
        if (!unlimitedTickTime)
        {
            if (!beginUpdateDone)
            {
                NodeManager.beginUpdate();
                beginUpdateDone = true;
                simStartTime = std::chrono::system_clock::now();
            }
            auto time = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time - simStartTime);
            float t = (float)((double)duration.count() / (double)tickTime.count());
            if (t > 1)
                t = 1;
            // update animation
            NodeManager.setProgress(t);
            // end cycle
            if (duration >= tickTime)
            {
                NodeManager.endUpdate();
                beginUpdateDone = false;
            }
        }
        else
        {
            NodeManager.beginUpdate();
            NodeManager.setProgress(1);
            NodeManager.endUpdate();
        }

        NodeManager.render(pmat, view, (invView * vec4(0, 0, 0, 1)).xy, (invView * vec4(width, height, 0, 1)).xy);

        if (boxSelection)
        {
            NodeManager.drawBoxSelection(boxSelectionStart, worldMouse, pmat, view);
        }

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
        long long finalElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(finalTime - startTime).count();
        int fps = (int)(1000000.0 / finalElapsedTime);
        string text = std::to_string(fps);
        font.text("fps : " + text, vec2(0), 20, vec3(1));

        double currentFps = (1000000.0 / finalElapsedTime);
        if (frame > 1)
        {
            avgFps = avgFps + (long double)(currentFps - avgFps) / (long double)frame;
        }
        else
        {
            avgFps = currentFps;
        }
        frame++;

        text = std::to_string((int)avgFps);
        font.text("avg fps : " + text, vec2(0, 20), 20, vec3(1));

        font.render(pmat);

        // End drawing
        platform.swapBuffers();
    }
    return 0;
}
