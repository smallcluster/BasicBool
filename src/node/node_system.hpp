#pragma once

#include "core/defines.hpp"
#include "core/math.hpp"
#include "look_and_feel.hpp"
#include <functional>
#include <optional>
#include <memory>

class Node;

struct Link;

struct Connector {
    Node *parent;
    std::vector<Link *> links;
    bool state = false;
    string name;
    vec2 pos;     // relative to parent node
    vec2 textPos; // relative to parent node
    bool isInput = false;

    Connector(string name, Node *parent, bool isInput);
};

// in -> out
struct Link {
    Connector *input;
    Connector *output;
    float t = 0.5f;
    bool state = false;
    bool nextState = false;

    Link(Connector *in, Connector *out);

    ~Link();

    void fetchNextState();

    void propagate();

    void reset();
};

class Node {
public:
    std::vector<Connector *> inputs;
    std::vector<Connector *> outputs;
    bool selected = false;
    string name;
    vec2 pos;
    vec2 textPos; // relative to it's pos
    vec2 headerSize;
    vec2 size;

    Node(string name, vec2 pos);

    ~Node();

    bool addOutput(string name);

    bool addInput(string name);

    Connector *getInput(string name);

    Connector *getOutput(string name);

    virtual void update() = 0;

    virtual void reset() = 0;
};


class NodeManager {
public:
    NodeManager();

    ~NodeManager();

    void addNode(Node *node);

    void addLink(Link *link);

    void render(const mat4 &pmat, const mat4 &view, const vec2 &camstart, const vec2 &camend);

    void beginUpdate();

    void endUpdate();

    void setProgress(float t);

    std::optional<Node *> getNodeAt(vec2 mouse);

    std::optional<Connector *> getConnectorAt(vec2 mouse);

    void drawTempLink(Connector *c, vec2 mouse, const mat4 &pmat, const mat4 &view);

    bool connect(Connector *c1, Connector *c2);

    void disconnectAll(Connector *c);

    void removeNode(Node *node);

    void setLookAndFeel(std::shared_ptr<NodeStyle> style);

    void reset();

    void moveSelectedNodes(vec2 offset);

    void boxSelect(vec2 start, vec2 end);

    void deselectAll();

    void selectNode(Node *node);

    void deselectNode(Node *node);

    bool nodeIsSelected(Node *node);

    void drawBoxSelection(vec2 start, vec2 end, const mat4 &pmat, const mat4 &view);

    void removeSelected();

    void replaceNode(Node* node, std::function<Node*(vec2)> build);
    void replaceSelected(std::function<Node*(vec2)> build);


private:
    std::vector<Node *> selectedNodes;

    std::vector<Node *> nodes;
    std::vector<Link *> links;

    // look and feel
    std::shared_ptr<NodeStyle> nodeStyle;

    // Layout building
    void doNodeLayout(Node *node);

    vec2 getConnectorSize(Node *node, bool output);

    void doConnectorLayout(Node *node, bool output);

    // shaders
    Shader nodeShader;
    Shader nodeShadowShader;
    Shader nodeConnectorShader;
    Shader linkShader;
    Shader basicShader;
    Shader boxSelectShader;
    // culling
    std::vector<Node *> visibleNodes;
    std::vector<Link *> visibleLinks;

    void cullNodes(const vec2 &camstart, const vec2 &camend);

    void cullLinks(const vec2 &camstart, const vec2 &camend);

    // batch rendering
    void renderShadows(const mat4 &pmat, const mat4 &view);

    void renderNodes(const mat4 &pmat, const mat4 &view);

    void renderText(const mat4 &pmat, const mat4 &view);

    void renderConnectors(const mat4 &pmat, const mat4 &view);

    void renderLinks(const mat4 &pmat, const mat4 &view);
};
