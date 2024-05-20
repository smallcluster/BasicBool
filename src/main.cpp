#include "core/defines.hpp"
#include "core/math.hpp"
#include "platform/platform.hpp"
#include "render/backend.hpp"
#include "render/text.hpp"
#include "node/node_system.hpp"
#include "node/nodes.hpp"
#include "gui/gui.hpp"
#include <chrono>
#include <ratio>
#include <thread>
#include <cmath>
#include <optional>

using namespace std::chrono_literals;


// ---------------------------------------------------------------------

//-----------------------------------------------------------------------
vec2 viewOffset = vec2(0);
float zoom = 1.0f;
vec2 oldMouse = vec2(0);
vec2 mouse = vec2(0);
bool viewPanning = false;

mat4 getViewMatrix() {
    mat4 view = scale(identity<4>(), vec3(zoom, zoom, 1));
    vec3 viewTranslate = vec3((1 - zoom) / 2.0f, (1 - zoom) / 2.0f, 0) + vec3(viewOffset, 0);
    view = translate(view, viewTranslate);
    return view;
}

mat4 getInvViewMatrix() {
    mat4 invView = scale(identity<4>(), vec3(1.0f / zoom, 1.0f / zoom, 1));
    vec3 viewTranslate = vec3((1 - zoom) / 2.0f, (1 - zoom) / 2.0f, 0) + vec3(viewOffset, 0);
    invView = translate(invView, -viewTranslate / zoom);
    return invView;
}

int main(int argc, char const *argv[]) {
    // Math vector & matrix packing check
    static_assert(sizeof(vec2) == sizeof(GLfloat) * 2, "Can't pack vec2.");
    static_assert(sizeof(vec3) == sizeof(GLfloat) * 3, "Can't pack vec3.");
    static_assert(sizeof(vec4) == sizeof(GLfloat) * 4, "Can't pack vec4.");
    static_assert(sizeof(mat4) == sizeof(GLfloat) * 16, "Can't pack mat4.");
    static_assert(sizeof(mat3) == sizeof(GLfloat) * 9, "Can't pack mat3.");

    Platform &platform = Platform::getInstance("BasicBool", 1280, 720);

    // Opengl Setup
    glDisable(GL_STENCIL_TEST);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(0);
    glDepthFunc(GL_ALWAYS);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Shaders
    Shader basicShader("basic");

    // Font
    Font &font = Font::getDefault();

    NodeManager NodeManager;

    unsigned int tickPerSec = 1;
    auto simStartTime = std::chrono::steady_clock::now();
    auto tickTime = 1000000000ns / tickPerSec;
    bool beginUpdateDone = false;
    bool boxSelection = false;
    vec2 boxSelectionStart = vec2(0);
    bool unlimitedTickTime = false;
    int fps = 60;

    vec2 orpos = vec2((float) platform.getWidth() / 2.0f, (float) platform.getHeight() / 2.0f);

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

    // GUI
    gui::GUIManager guiManager;
    

    // 0 -> action on node
    // 1 -> replace nodes
    // 2 -> add nodes
    // 3 -> File menu
    // 4 -> Edit menu
    // 5 -> Help menu
    int contextMenu = -1;
    vec2 contextMenuPos;
    std::optional<Node *> grabNode = {};
    std::optional<Connector *> startConnector = {};

    while (platform.processEvents()) {

        auto startTime = std::chrono::steady_clock::now();

        int width = platform.getWidth();
        int height = platform.getHeight();
        // update mouse pos
        oldMouse = mouse;
        vec2 oldWorldMouse = (getInvViewMatrix() * vec4(mouse, 0, 1)).xy;

        mouse = vec2((float) platform.getMouseX(), (float) platform.getMouseY());
        // move view
        if (viewPanning) {
            viewOffset = viewOffset + mouse - oldMouse;
        }
        int delta = platform.getMouseWheel();
        // update zoom
        if (delta != 0) {
            vec2 tempMouse = (getInvViewMatrix() * vec4(mouse, 0, 1)).xy;
            zoom += zoom * 0.05f * ((float) delta);
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
        mat4 pmat(vec4(2.0f / (float) width, 0, 0, 0), vec4(0, -2.0f / (float) height, 0, 0), vec4(0, 0, 1, 0),
                  vec4(-1, 1, 0, 1));

        if (platform.isMousePressed(MouseButton::LEFT) && contextMenu == -1) {
            startConnector = NodeManager.getConnectorAt(worldMouse);

            if (!startConnector) {
                grabNode = NodeManager.getNodeAt(worldMouse);
                if (grabNode) {
                    if (!NodeManager.nodeIsSelected(grabNode.value())) {
                        NodeManager.deselectAll();
                    }
                    NodeManager.selectNode(grabNode.value());
                } else {
                    NodeManager.deselectAll();
                    boxSelection = true;
                    boxSelectionStart = worldMouse;
                }
            }
        } else if (platform.isMouseReleased(MouseButton::LEFT)) {
            grabNode = {};
            if (startConnector) {
                auto endConnector = NodeManager.getConnectorAt(worldMouse);
                if (endConnector)
                    NodeManager.connect(startConnector.value(), endConnector.value());
                startConnector = {};
            }
            if (boxSelection) {
                boxSelection = false;
                vec2 boxSelectionEnd = worldMouse;
                vec2 boxStart = vec2(std::min(boxSelectionStart.x, boxSelectionEnd.x),
                                     std::min(boxSelectionStart.y, boxSelectionEnd.y));
                vec2 boxEnd = vec2(std::max(boxSelectionStart.x, boxSelectionEnd.x),
                                   std::max(boxSelectionStart.y, boxSelectionEnd.y));
                NodeManager.boxSelect(boxStart, boxEnd);
            }
        }

        if (platform.isMousePressed(MouseButton::RIGHT)) {
            auto c = NodeManager.getConnectorAt(worldMouse);
            if (c) {
                NodeManager.disconnectAll(c.value());
            } else {
                auto node = NodeManager.getNodeAt(worldMouse);
                if (node) {
                    if(!NodeManager.nodeIsSelected(node.value())){
                        NodeManager.deselectAll();
                        NodeManager.selectNode(node.value());
                    }
                    contextMenu = 0;
                    contextMenuPos = mouse;
                } else {
                    contextMenu = 2;
                    contextMenuPos = mouse;
                }
            }
        }

        if (platform.isMousePressed(MouseButton::MIDDLE)) {
            viewPanning = true;
        }
        if (platform.isMouseReleased(MouseButton::MIDDLE)) {
            viewPanning = false;
        }

        if (grabNode) {
            //grabNode.value()->pos = worldMouse + grabOffset;
            NodeManager.moveSelectedNodes(worldMouse - oldWorldMouse);
        }

        glViewport(0, 0, width, height);

        // Begin drawing

        // --- Background --- //
        float bgVal = 35.0f / 255.0f;
        glClearColor(bgVal, bgVal, bgVal, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // --- Grid --- //
        // TODO : improve this
        int nx = width / 16;
        int ny = height / 16;
        std::vector<vec2> data;
        data.reserve(nx + ny);
        for (int i = 0; i <= nx; ++i) {
            data.emplace_back(vec2(16.0f * ((float) i), 0));
            data.emplace_back(vec2(16.0f * ((float) i), (float) height));
        }
        for (int i = 0; i <= ny; ++i) {
            data.emplace_back(vec2(0, 16.0f * ((float) i)));
            data.emplace_back(vec2((float) width, 16.0f * ((float) i)));
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

        if (startConnector) {
            NodeManager.drawTempLink(startConnector.value(), worldMouse, pmat, view);
        }

        // ---- simulation ----



        NodeManager.render(pmat, view, (invView * vec4(0, 0, 0, 1)).xy,
                           (invView * vec4((float) width, (float) height, 0, 1)).xy);

        if (boxSelection) {
            NodeManager.drawBoxSelection(boxSelectionStart, worldMouse, pmat, view);
        }

        switch (contextMenu) {
            case 0:
            {
                static std::vector<string> list = {"Remove", "Replace"};
                int index = guiManager.dropDownMenu(list, contextMenuPos, pmat, {"Action menu"});
                if (platform.isMousePressed(MouseButton::LEFT)) {
                    contextMenu = -1;
                    vec2 pos = (invView*vec4(contextMenuPos, 0, 1)).xy;
                    switch (index) {
                        case 0:
                            NodeManager.removeSelected();
                            break;
                        case 1:
                            contextMenu = 1;
                            break;
                    }
                }
            }
                break;
            case 1:{
                static std::vector<string> list = {"TRUE", "NOT", "OR", "AND", "XOR"};
                int index = guiManager.dropDownMenu(list, contextMenuPos, pmat, {"Replace nodes menu"});
                if (platform.isMousePressed(MouseButton::LEFT)) {
                    contextMenu = -1;
                    switch (index) {
                        case 0:
                            NodeManager.replaceSelected([](vec2 pos){return new TrueNode(pos);});
                            break;
                        case 1:
                            NodeManager.replaceSelected([](vec2 pos){return new NotNode(pos);});
                            break;
                        case 2:
                            NodeManager.replaceSelected([](vec2 pos){return new OrNode(pos);});
                            break;
                        case 3:
                            NodeManager.replaceSelected([](vec2 pos){return new AndNode(pos);});
                            break;
                        case 4:
                            NodeManager.replaceSelected([](vec2 pos){return new XorNode(pos);});
                            break;
                    }
                }
            }
                break;
            case 2 : {
                static std::vector<string> list = {"TRUE", "NOT", "OR", "AND", "XOR", "PERFORMANCE TEST (40k)"};
                int index = guiManager.dropDownMenu(list, contextMenuPos, pmat, {"Add node menu"});
                if (platform.isMousePressed(MouseButton::LEFT)) {
                    contextMenu = -1;
                    vec2 pos = (invView*vec4(contextMenuPos, 0, 1)).xy;
                    switch (index) {
                        case 0:
                            NodeManager.addNode(new TrueNode(pos));
                            break;
                        case 1:
                            NodeManager.addNode(new NotNode(pos));
                            break;
                        case 2:
                            NodeManager.addNode(new OrNode(pos));
                            break;
                        case 3:
                            NodeManager.addNode(new AndNode(pos));
                            break;
                        case 4:
                            NodeManager.addNode(new XorNode(pos));
                            break;
                        case 5:
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
                            }
                            break;
                    }
                }
            } break;
            // File menu
            case 3 : {
                static std::vector<string> list = {"New", "Save", "Save As", "Exit"};
                int index = guiManager.dropDownMenu(list, contextMenuPos, pmat, {"File menu"});
                if (platform.isMousePressed(MouseButton::LEFT)) {
                    contextMenu = -1;
                }
            } break;
                // Edit menu
            case 4 : {
                static std::vector<string> list = {"Settings", "Cut", "Copy", "Delete"};
                int index = guiManager.dropDownMenu(list, contextMenuPos, pmat, {"Edit menu"});
                if (platform.isMousePressed(MouseButton::LEFT)) {
                    contextMenu = -1;
                }
            } break;
            // Help menu
            case 5 : {
                static std::vector<string> list = {"Check for Updates", "About"};
                int index = guiManager.dropDownMenu(list, contextMenuPos, pmat, {"Help menu"});
                if (platform.isMousePressed(MouseButton::LEFT)) {
                    contextMenu = -1;
                }
            } break;
        }

        static std::vector<string> fileMenu = {"File", "Edit", "Help"};
        auto info = guiManager.fileMenu(fileMenu, pmat);
        switch (info.first) {
            case 0:
                contextMenu = 3;
                contextMenuPos = info.second;
                break;
            case 1:
                contextMenu = 4;
                contextMenuPos = info.second;
                break;
            case 2:
                contextMenu = 5;
                contextMenuPos = info.second;
                break;
        }


        auto endTime = std::chrono::steady_clock::now();
        auto workTime = endTime-startTime;
        static auto targetTime = std::chrono::duration<long long , std::ratio<1, 60>>{1}; // 60 Hz

        // TODO : improve this busy update loop
        while (std::chrono::steady_clock::now()-startTime < targetTime){
            // start cycle
            if (!unlimitedTickTime) {
                if (!beginUpdateDone) {
                    NodeManager.beginUpdate();
                    beginUpdateDone = true;
                    simStartTime = std::chrono::steady_clock::now();
                }
                auto time = std::chrono::steady_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time - simStartTime);
                auto t = (float) ((double) duration.count() / (double) tickTime.count());
                if (t > 1)
                    t = 1;
                // update animation
                NodeManager.setProgress(t);
                // end cycle
                if (duration >= tickTime) {
                    NodeManager.endUpdate();
                    beginUpdateDone = false;
                }
            } else {
                NodeManager.beginUpdate();
                NodeManager.setProgress(1);
                NodeManager.endUpdate();
            }
        }

        string text = std::to_string(fps);
        font.text("fps : " + text, vec2(0, height-font.getHeight("fps : " + text, 20)), 20, vec3(1));
        font.render(pmat);

        // End drawing
        platform.swapBuffers();

        // FPS info
        auto afterSleep = std::chrono::steady_clock::now();
        long long finalElapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(afterSleep-startTime).count();
        fps = (int) (1000000.0 / (double) finalElapsedTime);
    }
    return 0;
}
