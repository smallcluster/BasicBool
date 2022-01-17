#pragma once
#include "core/defines.hpp"
#include "core/math.hpp"
#include <unordered_map>
#include "render/backend.hpp"
#include "render/text.hpp"
#include "render/shapes.hpp"
#include <cmath>

class Node;

struct Connector
{
    Node *parent;
    bool state = false;
    bool connected = false;
    string name;
    vec2 pos;
    Connector(string name, Node *parent) : name(name), parent(parent) {}
};

// in -> out
struct Link
{
    bool state = false;
    Connector *input;
    Connector *output;
    Link(Connector *in, Connector *out) : input(in), output(out)
    {
        input->connected = true;
        output->connected = true;
    }
    ~Link()
    {
        output->state = false;
    }
    void update()
    {
        output->state = input->state;
    }
    void reset()
    {
        input->state = false;
        output->state = false;
    }
};

class Node
{
    public:
        std::vector<Connector *> inputs;
        std::vector<Connector *> outputs;
    string name;
    vec2 pos;
    Node(string name, vec2 pos) : name(name), pos(pos) {}
    ~Node()
    {
        for(Connector *c : inputs)
            delete c;
        for(Connector *c : outputs)
            delete c;
    }
    bool addOutput(string name)
    {
        auto i = std::find_if(outputs.begin(), outputs.end(), [&name](Connector *c){return c->name == name;});
        if(i == outputs.end()){
            outputs.push_back(new Connector(name, this));
            return true;
        }
        return false;
    }
    bool addInput(string name)
    {
        auto i = std::find_if(inputs.begin(), inputs.end(), [&name](Connector *c){return c->name == name;});
        if(i == inputs.end()){
            inputs.push_back(new Connector(name, this));
            return true;
        }
        return false;
    }

    Connector *getInput(string name)
    {
        auto i = std::find_if(inputs.begin(), inputs.end(), [&name](Connector *c){return c->name == name;});
        if (i != inputs.end())
            return inputs[std::distance(inputs.begin(), i)];
        return nullptr;
    }

    Connector *getOutput(string name)
    {
        auto i = std::find_if(outputs.begin(), outputs.end(), [&name](Connector *c){return c->name == name;});
        if (i != outputs.end())
            return outputs[std::distance(outputs.begin(), i)];
        return nullptr;
    }



    virtual void update() = 0;
};

// ------ Basic nodes ---------------

class TrueNode : public Node
{
public:
    TrueNode(vec2 pos) : Node("TRUE", pos)
    {
        addOutput("out");
    }
    void update() override
    {
        getOutput("out")->state = true;
    }
};

class NotNode : public Node
{
public:
    NotNode(vec2 pos) : Node("NOT", pos)
    {
        addInput("in");
        addOutput("out");
    }
    void update() override
    {
        getOutput("out")->state = !(getInput("in")->state);
    }
};

class AndNode : public Node
{
public:
    AndNode(vec2 pos) : Node("AND", pos)
    {
        addInput("in1");
        addInput("in2");
        addOutput("out");
    }
    void update() override
    {
        getOutput("out")->state = getInput("in1")->state && getInput("in2")->state;
    }
};

class OrNode : public Node
{
public:
    OrNode(vec2 pos) : Node("OR", pos)
    {
        addInput("in1");
        addInput("in2");
        addOutput("out");
    }
    void update() override
    {
        getOutput("out")->state = getInput("in1")->state || getInput("in2")->state;
    }
};

struct ShadowDrawcall
{
    vec2 pos;
    vec2 size;
};

struct NodeDrawcall
{
    vec2 pos;
    vec2 size;
    float headerHeight;
    vec2 textPos;
    string text;
};

struct ConnectorDrawcall
{
    vec2 pos;
    float r;
    float state;
    vec2 textPos;
    string text;
};

// ---- Simulation ----
class NodeManager
{
private:
    const float headerTextSize = 18;
    const float connectorTextSize = 12;
    const float connectorRadius = 8;
    const vec2 connectorMargin = vec2(4, 4);
    const float inOutDist = 32;
    const float nodeRadius = 8;
    const float shadowSize = 16;
    const float textOutletMargin = 4;

    std::vector<Node *> nodes;

    // Drawcalls
    std::vector<ShadowDrawcall> shadowCalls;
    std::vector<NodeDrawcall> nodeCalls;
    std::vector<ConnectorDrawcall> connectorCalls;

    std::vector<Link *> links;
    Shader basicShader;
    Shader nodeShader;
    Shader nodeShadowShader;
    Shader nodeConnectorShader;
    Font &font;

    vec2 getConnectorSize(Node* node, bool output){
        vec2 size = vec2(0);
        std::vector<Connector*> &connectors = output ? node->outputs : node->inputs;
        for(Connector *c : connectors){
            size.y += connectorMargin.y*2+std::max(connectorRadius*2, font.getHeight(c->name, connectorTextSize));
            size.x = std::max(size.x, connectorMargin.x+connectorRadius*2+font.getWidth(c->name, connectorTextSize)+textOutletMargin);
        }
        return size;
    }

    void doConnectorLayout(Node* node, bool output, float nodeWidth, float headerHeight){
            float inOffsetY = node->pos.y+headerHeight;
            std::vector<Connector*> &connectors = output ? node->outputs : node->inputs;

            for(Connector *c : connectors){
                inOffsetY += connectorMargin.y;

                vec2 outletPos;
                vec2 textPos;

                if(output){
                    float textWidth = font.getWidth(c->name, connectorTextSize);
                    outletPos.x = node->pos.x + nodeWidth - connectorMargin.x - connectorRadius;
                    textPos.x = node->pos.x + nodeWidth - connectorMargin.x - 2*connectorRadius - textWidth-textOutletMargin;
                } else {
                    outletPos.x = node->pos.x + connectorMargin.x+connectorRadius;
                    textPos.x = node->pos.x + connectorMargin.x+2*connectorRadius+textOutletMargin;
                }

                float textHeight = font.getHeight(c->name, connectorTextSize);
                float totalHeight = textHeight;
                if(connectorRadius*2 <= textHeight){
                    textPos.y = inOffsetY;
                    outletPos.y = inOffsetY+textHeight/2.0f;
                } else {
                    outletPos.y = inOffsetY+connectorRadius;
                    textPos.y = inOffsetY+connectorRadius-textHeight/2.0f;
                    totalHeight = connectorRadius*2;
                }

                // update connector pos
                c->pos = outletPos;
                // draw connector
                connectorCalls.push_back({outletPos, connectorRadius, !(c->connected) ? -1.0f : c->state ? 1.0f : 0.0f, textPos, c->name});

                inOffsetY += totalHeight;
            }
    }

    void doNodeLayout()
    {
        for (Node *node : nodes)
        {
            // node
            const float headerHeight = font.getHeight(node->name, headerTextSize) + nodeRadius;
            const float headerWidth = font.getWidth(node->name, headerTextSize) + 2 * nodeRadius;

            vec2 inputsSize = getConnectorSize(node, false);
            vec2 outputsSize = getConnectorSize(node, true);


            vec2 nodeSize = vec2(std::max(headerWidth, inputsSize.x+inOutDist+outputsSize.x), headerHeight+std::max(inputsSize.y,outputsSize.y));

            nodeCalls.push_back({node->pos, nodeSize, headerHeight, node->pos+vec2(nodeRadius, nodeRadius), node->name});

            // shadow
            shadowCalls.push_back({node->pos-vec2(shadowSize, shadowSize), nodeSize+2*vec2(shadowSize, shadowSize)});

            // inputs
            doConnectorLayout(node, false, nodeSize.x, headerHeight);
            // outputs
            doConnectorLayout(node, true, nodeSize.x, headerHeight);
        }
    }

    void renderShadows(const mat4 &pmat)
    {
        if(shadowCalls.empty())
            return;
        std::vector<float> vertices;
        vertices.reserve(shadowCalls.size()*6);
        std::vector<unsigned int> indices;
        indices.reserve(shadowCalls.size()*6);
        int i = 0;
        for(const ShadowDrawcall &c : shadowCalls){
            vertices.insert(vertices.end(), {
                // pos                                 // uv  // size
                c.pos.x, c.pos.y,                      0, 0,  c.size.x, c.size.y,
                c.pos.x+c.size.x, c.pos.y,             1, 0,  c.size.x, c.size.y,
                c.pos.x+c.size.x, c.pos.y+c.size.y,    1, 1,  c.size.x, c.size.y,
                c.pos.x, c.pos.y+c.size.y,             0, 1,  c.size.x, c.size.y
            });
            unsigned int n = (i++)*4;
            indices.insert(indices.end(), { n, n+1, n+2, n, n+2, n+3});
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float)*vertices.size());
        ElementBuffer ebo(&indices[0], sizeof(unsigned int)*indices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(2); // uv
        layout.push<float>(2); // size
        vao.addBuffer(vbo, layout);
        vao.addBuffer(ebo);
        nodeShadowShader.use();
        nodeShadowShader.setMat4("projection", pmat);
        nodeShadowShader.setMat4("transform", identity<4>());
        nodeShadowShader.setFloat("smoothing", shadowSize);
        nodeShadowShader.setFloat("radius", nodeRadius);
        glDrawElements(GL_TRIANGLES, shadowCalls.size() * 6, GL_UNSIGNED_INT, 0);
        shadowCalls.clear();
    }

    void renderNodes(const mat4 &pmat)
    {
        if(nodeCalls.empty())
            return;
        std::vector<float> vertices;
        vertices.reserve(nodeCalls.size()*7);
        std::vector<unsigned int> indices;
        indices.reserve(nodeCalls.size()*6);
        int i = 0;
        for(const NodeDrawcall &c : nodeCalls){
            vertices.insert(vertices.end(), {
                // pos                                 // uv  // size               // header height
                c.pos.x, c.pos.y,                      0, 0,  c.size.x, c.size.y,   c.headerHeight,
                c.pos.x+c.size.x, c.pos.y,             1, 0,  c.size.x, c.size.y,   c.headerHeight,
                c.pos.x+c.size.x, c.pos.y+c.size.y,    1, 1,  c.size.x, c.size.y,   c.headerHeight,
                c.pos.x, c.pos.y+c.size.y,             0, 1,  c.size.x, c.size.y,   c.headerHeight
            });
            unsigned int n = (i++)*4;
            indices.insert(indices.end(), { n, n+1, n+2, n, n+2, n+3});
            font.text(c.text, c.textPos, headerTextSize, vec3(1)); // register text
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float)*vertices.size());
        ElementBuffer ebo(&indices[0], sizeof(unsigned int)*indices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(2); // uv
        layout.push<float>(2); // size
        layout.push<float>(1); // headerHeight
        vao.addBuffer(vbo, layout);
        vao.addBuffer(ebo);
        nodeShader.use();
        nodeShader.setMat4("projection", pmat);
        nodeShader.setMat4("transform", identity<4>());
        nodeShader.setFloat("radius", nodeRadius);
        glDrawElements(GL_TRIANGLES, nodeCalls.size() * 6, GL_UNSIGNED_INT, 0);
        nodeCalls.clear();
    }

    void renderConnectors(const mat4 &pmat)
    {
        if(connectorCalls.empty())
            return;
        std::vector<float> vertices;
        vertices.reserve(connectorCalls.size()*6);
        std::vector<unsigned int> indices;
        indices.reserve(connectorCalls.size()*6);
        int i = 0;
        for(const ConnectorDrawcall &c : connectorCalls){
            vertices.insert(vertices.end(), {
                // pos                      // uv   // radius   // state
                c.pos.x-c.r, c.pos.y-c.r,   0, 0,   c.r,        c.state,
                c.pos.x+c.r, c.pos.y-c.r,   1, 0,   c.r,        c.state,
                c.pos.x+c.r, c.pos.y+c.r,   1, 1,   c.r,        c.state,
                c.pos.x-c.r, c.pos.y+c.r,   0, 1,   c.r,        c.state,
            });
            unsigned int n = (i++)*4;
            indices.insert(indices.end(), { n, n+1, n+2, n, n+2, n+3});
            font.text(c.text, c.textPos, connectorTextSize, vec3(1)); // register text
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float)*vertices.size());
        ElementBuffer ebo(&indices[0], sizeof(unsigned int)*indices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(2); // uv
        layout.push<float>(1); // radius
        layout.push<float>(1); // state
        vao.addBuffer(vbo, layout);
        vao.addBuffer(ebo);
        nodeConnectorShader.use();
        nodeConnectorShader.setMat4("projection", pmat);
        nodeConnectorShader.setMat4("transform", identity<4>());
        glDrawElements(GL_TRIANGLES, connectorCalls.size() * 6, GL_UNSIGNED_INT, 0);
        connectorCalls.clear();
    }

public:
    NodeManager(Font &font) : basicShader("basic"),
                              nodeShader("node"),
                              nodeShadowShader("node_shadow"),
                              nodeConnectorShader("node_connector"),
                              font(font)
    {
    }

    bool addNode(string name, vec2 pos)
    {
        if (name == "TRUE")
        {
            nodes.push_back(new TrueNode(pos));
            return true;
        }
        else if (name == "NOT")
        {
            nodes.push_back(new NotNode(pos));
            return true;
        }
        else if (name == "AND")
        {
            nodes.push_back(new AndNode(pos));
            return true;
        }
        else if (name == "OR")
        {
            nodes.push_back(new OrNode(pos));
            return true;
        }
        return false;
    }
    void render(const mat4 &pmat)
    {
        doNodeLayout();
        // TODO : render links
        renderShadows(pmat);
        renderNodes(pmat);
        renderConnectors(pmat);
        // render text
        font.render(pmat);
    }

    void simulate()
    {
        // Nodes THEN links
        for (Node *n : nodes)
            n->update();
        for (Link *li : links)
            li->update();
    }
};
