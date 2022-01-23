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
    Connector(string name, Node *parent, bool isInput) : name(name), parent(parent), isInput(isInput) {}
};

// in -> out
struct Link
{
    Connector *input;
    Connector *output;
    Link(Connector *in, Connector *out) : input(in), output(out)
    {
        in->links.push_back(this);
        out->links.push_back(this);
    }
    ~Link()
    {
        output->state = false;
        input->links.erase(std::remove(input->links.begin(), input->links.end(), this), input->links.end());
        output->links.erase(std::remove(output->links.begin(), output->links.end(), this), output->links.end());
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

struct NodeStyle
{
    float headerTextSize = 20;
    float connectorTextSize = 16;
    float connectorRadius = 8;
    vec2 connectorMargin = vec2(4, 4);
    float inOutDist = 32;
    float nodeRadius = 8;
    float nodeRediusOver = 10;
    float shadowSize = 16;
    float textOutletMargin = 4;
    Font &font;
    NodeStyle(Font &font) : font(font) {}
    NodeStyle() : font(Font::getDefault()) {}

    static NodeStyle &getDefault()
    {
        static NodeStyle defaultstyle;
        return defaultstyle;
    }
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
    Node(string name, vec2 pos, NodeStyle &style) : name(name), pos(pos), style(style)
    {
    }

    Node(string name, vec2 pos) : name(name), pos(pos), style(NodeStyle::getDefault())
    {
    }

    ~Node()
    {
        for (Connector *c : inputs)
            delete c;
        for (Connector *c : outputs)
            delete c;
    }

    bool addOutput(string name)
    {
        auto i = std::find_if(outputs.begin(), outputs.end(), [&name](Connector *c)
                              { return c->name == name; });
        if (i == outputs.end())
        {
            outputs.push_back(new Connector(name, this, false));
            doLayout();
            return true;
        }
        return false;
    }
    bool addInput(string name)
    {
        auto i = std::find_if(inputs.begin(), inputs.end(), [&name](Connector *c)
                              { return c->name == name; });
        if (i == inputs.end())
        {
            inputs.push_back(new Connector(name, this, true));
            doLayout();
            return true;
        }
        return false;
    }

    Connector *getInput(string name)
    {
        auto i = std::find_if(inputs.begin(), inputs.end(), [&name](Connector *c)
                              { return c->name == name; });
        if (i != inputs.end())
            return inputs[std::distance(inputs.begin(), i)];
        return nullptr;
    }

    Connector *getOutput(string name)
    {
        auto i = std::find_if(outputs.begin(), outputs.end(), [&name](Connector *c)
                              { return c->name == name; });
        if (i != outputs.end())
            return outputs[std::distance(outputs.begin(), i)];
        return nullptr;
    }

    virtual void update() = 0;

    vec2 getTextPos() const
    {
        return pos + textPos;
    }

private:
    NodeStyle &style;
    vec2 textPos; // relative to it's pos

    void doLayout()
    {
        const float headerHeight = style.font.getHeight(name, style.headerTextSize) + style.nodeRadius;
        const float headerWidth = style.font.getWidth(name, style.headerTextSize) + 2 * style.nodeRadius;
        textPos = vec2(style.nodeRadius, style.nodeRadius);
        vec2 inputsSize = getConnectorSize(false);
        vec2 outputsSize = getConnectorSize(true);
        size = vec2(std::max(headerWidth, inputsSize.x + style.inOutDist + outputsSize.x), headerHeight + std::max(inputsSize.y, outputsSize.y));

        headerSize = vec2(size.x, headerHeight);
        doConnectorLayout(false);
        doConnectorLayout(true);
    }
    vec2 getConnectorSize(bool output)
    {
        vec2 connectorSize = vec2(0);
        std::vector<Connector *> &connectors = output ? outputs : inputs;
        for (Connector *c : connectors)
        {
            connectorSize.y += style.connectorMargin.y * 2 + std::max(style.connectorRadius * 2, style.font.getHeight(c->name, style.connectorTextSize));
            connectorSize.x = std::max(connectorSize.x, style.connectorMargin.x + style.connectorRadius * 2 + style.font.getWidth(c->name, style.connectorTextSize) + style.textOutletMargin);
        }
        return connectorSize;
    }
    void doConnectorLayout(bool output)
    {
        float inOffsetY = headerSize.y;
        std::vector<Connector *> &connectors = output ? outputs : inputs;
        for (Connector *c : connectors)
        {
            inOffsetY += style.connectorMargin.y;
            vec2 outletPos;
            vec2 connectorTextPos;
            if (output)
            {
                float textWidth = style.font.getWidth(c->name, style.connectorTextSize);
                outletPos.x = size.x - style.connectorMargin.x - style.connectorRadius;
                connectorTextPos.x = size.x - style.connectorMargin.x - 2 * style.connectorRadius - textWidth - style.textOutletMargin;
            }
            else
            {
                outletPos.x = style.connectorMargin.x + style.connectorRadius;
                connectorTextPos.x = style.connectorMargin.x + 2 * style.connectorRadius + style.textOutletMargin;
            }
            float textHeight = style.font.getHeight(c->name, style.connectorTextSize);
            float totalHeight = textHeight;
            if (style.connectorRadius * 2 <= textHeight)
            {
                connectorTextPos.y = inOffsetY;
                outletPos.y = inOffsetY + textHeight / 2.0f;
            }
            else
            {
                outletPos.y = inOffsetY + style.connectorRadius;
                connectorTextPos.y = inOffsetY + style.connectorRadius - textHeight / 2.0f;
                totalHeight = style.connectorRadius * 2;
            }
            // update connector pos
            c->pos = outletPos;
            c->textPos = connectorTextPos;
            inOffsetY += totalHeight;
        }
    }
};

// ------ Basic nodes ---------------

class TrueNode : public Node
{
public:
    TrueNode(vec2 pos, NodeStyle &style) : Node("TRUE", pos, style)
    {
        addOutput("out");
    }
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
    NotNode(vec2 pos, NodeStyle &style) : Node("NOT", pos, style)
    {
        addInput("in");
        addOutput("out");
    }
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
    AndNode(vec2 pos, NodeStyle &style) : Node("AND", pos, style)
    {
        addInput("in1");
        addInput("in2");
        addOutput("out");
    }
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
    OrNode(vec2 pos, NodeStyle &style) : Node("OR", pos, style)
    {
        addInput("in1");
        addInput("in2");
        addOutput("out");
    }
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

// ---- Simulation ----
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

    void cullNodes(const vec2 &camstart, const vec2 &camend)
    {
        for (Node *node : nodes)
        {
            vec2 pos = node->pos - vec2(nodeStyle.shadowSize);
            vec2 size = node->size + vec2(2 * nodeStyle.shadowSize);
            if (pos.x + size.x >= camstart.x && pos.x <= camend.x && pos.y + size.y >= camstart.y && pos.y <= camend.y)
                visibleNodes.push_back(node);
        }
    }

    void cullLinks(const vec2 &camstart, const vec2 &camend)
    {
        for (Link *link : links)
        {
            vec2 start = link->input->parent->pos + link->input->pos;
            vec2 end = link->output->parent->pos + link->output->pos;
            // construct bounding box
            vec2 boxstart = vec2(std::min(start.x, end.x), std::min(start.y, end.y));
            vec2 boxeend = vec2(std::max(start.x, end.x), std::max(start.y, end.y));
            if (boxstart.x <= camend.x && boxstart.y <= camend.y && boxeend.x >= camstart.x && boxeend.y >= camstart.y)
                visibleLinks.push_back(link);
        }
    }

    void renderShadows(const mat4 &pmat, const mat4 &view)
    {
        if (visibleNodes.empty())
            return;

        std::vector<float> vertices;
        vertices.reserve(visibleNodes.size() * 6);
        std::vector<unsigned int> indices;
        indices.reserve(visibleNodes.size() * 6);
        int i = 0;
        for (const Node *node : visibleNodes)
        {
            vec2 pos = node->pos - vec2(nodeStyle.shadowSize);
            vec2 size = node->size + vec2(nodeStyle.shadowSize * 2);
            vertices.insert(vertices.end(), {// pos                                               // uv // size
                                             pos.x, pos.y, 0, 0, size.x, size.y,
                                             pos.x + size.x, pos.y, 1, 0, size.x, size.y,
                                             pos.x + size.x, pos.y + size.y, 1, 1, size.x, size.y,
                                             pos.x, pos.y + size.y, 0, 1, size.x, size.y});
            unsigned int n = (i++) * 4;
            indices.insert(indices.end(), {n, n + 1, n + 2, n, n + 2, n + 3});
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
        ElementBuffer ebo(&indices[0], sizeof(unsigned int) * indices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(2); // uv
        layout.push<float>(2); // size
        vao.addBuffer(vbo, layout);
        vao.addBuffer(ebo);
        nodeShadowShader.use();
        nodeShadowShader.setMat4("projection", pmat);
        nodeShadowShader.setMat4("view", view);
        nodeShadowShader.setFloat("smoothing", nodeStyle.shadowSize);
        nodeShadowShader.setFloat("radius", nodeStyle.nodeRadius);
        glDrawElements(GL_TRIANGLES, visibleNodes.size() * 6, GL_UNSIGNED_INT, 0);
    }

    void renderNodes(const mat4 &pmat, const mat4 &view)
    {
        if (visibleNodes.empty())
            return;
        std::vector<float> vertices;
        vertices.reserve(visibleNodes.size() * 7);
        std::vector<unsigned int> indices;
        indices.reserve(visibleNodes.size() * 6);
        int i = 0;
        for (const Node *node : visibleNodes)
        {
            vec2 pos = node->pos;
            vec2 size = node->size;
            float headerHeight = node->headerSize.y;
            vertices.insert(vertices.end(), {// pos                                 // uv  // size               // header height
                                             pos.x, pos.y, 0, 0, size.x, size.y, headerHeight,
                                             pos.x + size.x, pos.y, 1, 0, size.x, size.y, headerHeight,
                                             pos.x + size.x, pos.y + size.y, 1, 1, size.x, size.y, headerHeight,
                                             pos.x, pos.y + size.y, 0, 1, size.x, size.y, headerHeight});
            unsigned int n = (i++) * 4;
            indices.insert(indices.end(), {n, n + 1, n + 2, n, n + 2, n + 3});
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
        ElementBuffer ebo(&indices[0], sizeof(unsigned int) * indices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(2); // uv
        layout.push<float>(2); // size
        layout.push<float>(1); // headerHeight
        vao.addBuffer(vbo, layout);
        vao.addBuffer(ebo);
        nodeShader.use();
        nodeShader.setMat4("projection", pmat);
        nodeShader.setMat4("view", view);
        nodeShader.setFloat("radius", nodeStyle.nodeRadius);
        glDrawElements(GL_TRIANGLES, visibleNodes.size() * 6, GL_UNSIGNED_INT, 0);
    }

    void renderText(const mat4 &pmat, const mat4 &view)
    {
        if (visibleNodes.empty())
            return;
        for (const Node *node : visibleNodes)
        {
            nodeStyle.font.text(node->name, node->getTextPos(), nodeStyle.headerTextSize, vec3(1));
            for (const Connector *c : node->inputs)
            {
                nodeStyle.font.text(c->name, c->parent->pos + c->textPos, nodeStyle.connectorTextSize, vec3(1));
            }
            for (const Connector *c : node->outputs)
            {
                nodeStyle.font.text(c->name, c->parent->pos + c->textPos, nodeStyle.connectorTextSize, vec3(1));
            }
        }
        nodeStyle.font.render(pmat, view);
    }

    void renderConnectors(const mat4 &pmat, const mat4 &view)
    {
        if (visibleNodes.empty())
            return;

        std::vector<float> vertices;
        std::vector<unsigned int> indices;
        int i = 0;
        for (const Node *node : visibleNodes)
        {

            for (const Connector *c : node->inputs)
            {
                vec2 pos = c->parent->pos + c->pos;
                float state = c->state ? 1.0f : 0.0f;
                vertices.insert(vertices.end(), {
                                                    // pos                      // uv   // radius   // state
                                                    pos.x - nodeStyle.connectorRadius,
                                                    pos.y - nodeStyle.connectorRadius,
                                                    0,
                                                    0,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                    pos.x + nodeStyle.connectorRadius,
                                                    pos.y - nodeStyle.connectorRadius,
                                                    1,
                                                    0,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                    pos.x + nodeStyle.connectorRadius,
                                                    pos.y + nodeStyle.connectorRadius,
                                                    1,
                                                    1,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                    pos.x - nodeStyle.connectorRadius,
                                                    pos.y + nodeStyle.connectorRadius,
                                                    0,
                                                    1,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                });
                unsigned int n = (i++) * 4;
                indices.insert(indices.end(), {n, n + 1, n + 2, n, n + 2, n + 3});
            }
            for (const Connector *c : node->outputs)
            {
                vec2 pos = c->parent->pos + c->pos;
                float state = c->state ? 1.0f : 0.0f;
                vertices.insert(vertices.end(), {
                                                    // pos                      // uv   // radius   // state
                                                    pos.x - nodeStyle.connectorRadius,
                                                    pos.y - nodeStyle.connectorRadius,
                                                    0,
                                                    0,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                    pos.x + nodeStyle.connectorRadius,
                                                    pos.y - nodeStyle.connectorRadius,
                                                    1,
                                                    0,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                    pos.x + nodeStyle.connectorRadius,
                                                    pos.y + nodeStyle.connectorRadius,
                                                    1,
                                                    1,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                    pos.x - nodeStyle.connectorRadius,
                                                    pos.y + nodeStyle.connectorRadius,
                                                    0,
                                                    1,
                                                    nodeStyle.connectorRadius,
                                                    state,
                                                });
                unsigned int n = (i++) * 4;
                indices.insert(indices.end(), {n, n + 1, n + 2, n, n + 2, n + 3});
            }
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
        ElementBuffer ebo(&indices[0], sizeof(unsigned int) * indices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(2); // uv
        layout.push<float>(1); // radius
        layout.push<float>(1); // state
        vao.addBuffer(vbo, layout);
        vao.addBuffer(ebo);
        nodeConnectorShader.use();
        nodeConnectorShader.setMat4("projection", pmat);
        nodeConnectorShader.setMat4("view", view);
        glDrawElements(GL_TRIANGLES, i * 6, GL_UNSIGNED_INT, 0);
    }

    void renderLinks(const mat4 &pmat, const mat4 &view)
    {
        if (visibleLinks.empty())
            return;
        std::vector<float> vertices;
        vertices.reserve(visibleLinks.size() * 6);

        for (const Link *li : visibleLinks)
        {
            float state = li->input->state ? 1.0f : 0.0f;

            vec2 inPos = li->input->parent->pos + li->input->pos;
            vec2 outPos = li->output->parent->pos + li->output->pos;
            vertices.insert(vertices.end(), {// pos                                  // state
                                             inPos.x, inPos.y, state,
                                             outPos.x, outPos.y, state});
        }
        VertexArray vao;
        VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
        VertexBufferLayout layout;
        layout.push<float>(2); // pos
        layout.push<float>(1); // state
        vao.addBuffer(vbo, layout);

        linkShader.use();
        linkShader.setMat4("projection", pmat);
        linkShader.setMat4("view", view);
        linkShader.setFloat("width", nodeStyle.connectorRadius / 2.0f);
        glDrawArrays(GL_LINES, 0, 2 * visibleLinks.size());
    }

public:
    NodeManager() : nodeShader("node"),
                    nodeShadowShader("node_shadow"),
                    nodeConnectorShader("node_connector"),
                    linkShader("link"),
                    basicShader("basic"),
                    nodeStyle(NodeStyle::getDefault())
    {
    }

    ~NodeManager()
    {
        // LINKS FIRST
        for (Link *link : links)
            delete link;
        for (Node *node : nodes)
            delete node;
    }

    void addNode(Node *node)
    {
        nodes.push_back(node);
    }

    void addLink(Link *link)
    {
        links.push_back(link);
    }

    bool createNode(string name, vec2 pos)
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

    void render(const mat4 &pmat, const mat4 &view, const vec2 &camstart, const vec2 &camend)
    {
        cullNodes(camstart, camend);
        cullLinks(camstart, camend);

        renderLinks(pmat, view);
        renderShadows(pmat, view);
        renderNodes(pmat, view);
        renderConnectors(pmat, view);
        renderText(pmat, view);

        // TODO : remove this
        nodeStyle.font.text("Visible nodes : " + std::to_string(visibleNodes.size()), vec2(0, 40), 20, vec3(0));
        nodeStyle.font.text("Visible links : " + std::to_string(visibleLinks.size()), vec2(0, 60), 20, vec3(0));
        nodeStyle.font.render(pmat);

        visibleNodes.clear();
        visibleLinks.clear();
    }

    void simulate()
    {

        // Nodes THEN links

#if MULTI_CORES

#pragma omp parallel for
        for (int i = 0; i < nodes.size(); i++)
        {
            nodes[i]->update();
        }

#pragma omp parallel for
        for (int i = 0; i < links.size(); i++)
        {
            links[i]->update();
        }

#else

        for (Node *n : nodes)
            n->update();
        for (Link *li : links)
            li->update();

#endif
   
    }

    std::optional<Node *> getNodeAt(vec2 mouse)
    {
        for (Node *node : nodes)
        {
            vec2 size = node->headerSize;
            vec2 pos = node->pos;
            if (mouse.x >= pos.x && mouse.x <= pos.x + size.x && mouse.y >= pos.y && mouse.y <= pos.y + size.y)
                return node;
        }
        return {};
    }

    std::optional<Connector *> getConnectorAt(vec2 mouse)
    {
        for (Node *node : nodes)
        {
            vec2 size = node->size;
            vec2 pos = node->pos;
            if (mouse.x >= pos.x && mouse.x <= pos.x + size.x && mouse.y >= pos.y && mouse.y <= pos.y + size.y)
            {
                // check inputs
                for (Connector *c : node->inputs)
                {
                    vec2 cpos = node->pos + c->pos;
                    float r = nodeStyle.connectorRadius;
                    if (distanceSq(mouse, cpos) <= r * r)
                        return c;
                }

                // check outputs
                for (Connector *c : node->outputs)
                {
                    vec2 cpos = node->pos + c->pos;
                    float r = nodeStyle.connectorRadius;
                    if (distanceSq(mouse, cpos) <= r * r)
                        return c;
                }
            }
        }
        return {};
    }

    void drawTempLink(Connector *c, vec2 mouse, const mat4 &pmat, const mat4 &view)
    {

        vec2 start;
        vec2 end;

        if (c->isInput)
        {
            start = mouse;
            end = c->parent->pos + c->pos;
        }
        else
        {
            start = c->parent->pos + c->pos;
            end = mouse;
        }

        float state = c->state && !c->isInput ? 1.0f : 0.0f;

        float sv[] = {
            start.x, start.y, state,
            end.x, end.y, state};

        VertexArray svao;
        VertexBuffer svbo(sv, sizeof(sv));
        VertexBufferLayout slayout;
        slayout.push<float>(2);
        slayout.push<float>(1);
        svao.addBuffer(svbo, slayout);
        linkShader.use();
        linkShader.setMat4("projection", pmat);
        linkShader.setMat4("view", view);
        linkShader.setFloat("width", nodeStyle.connectorRadius / 2.0f);
        glDrawArrays(GL_LINES, 0, 2);
    }

    bool connect(Connector *c1, Connector *c2)
    {
        if (!c1->isInput && c2->isInput)
        {
            if (!(c2->links.empty()))
            {
                Link *li = c2->links[0];
                links.erase(std::remove(links.begin(), links.end(), li), links.end());
                delete li;
            }
            links.push_back(new Link(c1, c2));
            return true;
        }
        else if (c1->isInput && !c2->isInput)
        {

            if (!(c1->links.empty()))
            {
                Link *li = c1->links[0];
                links.erase(std::remove(links.begin(), links.end(), li), links.end());
                delete li;
            }
            links.push_back(new Link(c2, c1));
            return true;
        }
        return false;
    }

    void disconnectAll(Connector *c)
    {
        std::vector<Link *> temp = c->links;
        for (Link *li : temp)
        {
            links.erase(std::remove(links.begin(), links.end(), li), links.end());
            delete li;
        }
    }

    void removeNode(Node *node)
    {
        // remove & delete links
        for (Connector *c : node->inputs)
            disconnectAll(c);
        for (Connector *c : node->outputs)
            disconnectAll(c);
        // remove & delete node
        nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
        delete node;
    }
};
