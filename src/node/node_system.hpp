#pragma once
#include "core/defines.hpp"
#include "core/math.hpp"
#include <unordered_map>
#include "render/backend.hpp"
#include "render/text.hpp"
#include "render/shapes.hpp"
#include <cmath>
#include <optional>

class Node;
struct Link;

struct Connector
{
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
struct Link
{
    Connector *input;
    Connector *output;
    float t = 0.5f;
    bool state = false;
    bool nextState = false;
    Link(Connector *in, Connector *out);
    ~Link();
    void fetchNextState();
    void propagate();
};

struct NodeStyle
{
    vec3 trueColor = vec3(0, 1, 0);
    vec3 falseColor = vec3(1, 0, 0);
    vec3 headerColor = vec3(0, 0, 1);
    vec3 bodyColor = vec3(0.2);
    vec3 headerTextColor = vec3(1);
    vec3 connectorTextColor = vec3(1);
    float headerTextSize = 20;
    float connectorTextSize = 16;
    float connectorRadius = 8;
    vec2 connectorMargin = vec2(4, 4);
    float inOutDist = 32;
    float nodeRadius = 8;
    float shadowSize = 16;
    float textOutletMargin = 4;
    Font &font;

    NodeStyle(Font &font);
    NodeStyle();
    static NodeStyle &getDefault();
};

class Node
{

public:
    std::vector<Connector *> inputs;
    std::vector<Connector *> outputs;
    string name;
    vec2 pos;
    vec2 headerSize;
    vec2 size;
    Node(string name, vec2 pos, NodeStyle &style);
    Node(string name, vec2 pos);
    ~Node();

    bool addOutput(string name);
    bool addInput(string name);
    Connector *getInput(string name);
    Connector *getOutput(string name);
    vec2 getTextPos() const;

    virtual void update() = 0;

private:
    NodeStyle &style;
    vec2 textPos; // relative to it's pos
    void doLayout();
    vec2 getConnectorSize(bool output);
    void doConnectorLayout(bool output);
};

class NodeManager
{
private:
    std::vector<Node *> nodes;
    std::vector<Link *> links;
    Shader nodeShader;
    Shader nodeShadowShader;
    Shader nodeConnectorShader;
    Shader linkShader;
    Shader basicShader;
    NodeStyle &nodeStyle;
    std::vector<Node *> visibleNodes;
    std::vector<Link *> visibleLinks;
    void cullNodes(const vec2 &camstart, const vec2 &camend);
    void cullLinks(const vec2 &camstart, const vec2 &camend);
    void renderShadows(const mat4 &pmat, const mat4 &view);
    void renderNodes(const mat4 &pmat, const mat4 &view);
    void renderText(const mat4 &pmat, const mat4 &view);
    void renderConnectors(const mat4 &pmat, const mat4 &view);
    void renderLinks(const mat4 &pmat, const mat4 &view);

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

};
