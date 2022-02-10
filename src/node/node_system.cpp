#include "node_system.hpp"
#include "render/backend.hpp"
#include <cmath>
#include <optional>


// Connector
Connector::Connector(string name, Node *parent, bool isInput) : name(name), parent(parent), isInput(isInput) {}

// Links
Link::Link(Connector *in, Connector *out) : input(in), output(out) {
    in->links.push_back(this);
    out->links.push_back(this);
    fetchNextState();
}

Link::~Link() {
    input->links.erase(std::remove(input->links.begin(), input->links.end(), this), input->links.end());
    output->links.erase(std::remove(output->links.begin(), output->links.end(), this), output->links.end());
    output->state = false;
}

void Link::fetchNextState() {
    nextState = input->state;
}

void Link::propagate() {
    output->state = nextState;
    state = nextState; // current state
}

void Link::reset() {
    state = false;
    fetchNextState();
}

// Node
Node::Node(string name, vec2 pos) : name(name), pos(pos) {}

Node::~Node() {
    for (Connector *c: inputs)
        delete c;
    for (Connector *c: outputs)
        delete c;
}

bool Node::addOutput(string name) {
    auto i = std::find_if(outputs.begin(), outputs.end(), [&name](Connector *c) { return c->name == name; });
    if (i == outputs.end()) {
        outputs.push_back(new Connector(name, this, false));
        return true;
    }
    return false;
}

bool Node::addInput(string name) {
    auto i = std::find_if(inputs.begin(), inputs.end(), [&name](Connector *c) { return c->name == name; });
    if (i == inputs.end()) {
        inputs.push_back(new Connector(name, this, true));
        return true;
    }
    return false;
}

Connector *Node::getInput(string name) {
    auto i = std::find_if(inputs.begin(), inputs.end(), [&name](Connector *c) { return c->name == name; });
    if (i != inputs.end())
        return inputs[std::distance(inputs.begin(), i)];
    return nullptr;
}

Connector *Node::getOutput(string name) {
    auto i = std::find_if(outputs.begin(), outputs.end(), [&name](Connector *c) { return c->name == name; });
    if (i != outputs.end())
        return outputs[std::distance(outputs.begin(), i)];
    return nullptr;
}

// Node manager

NodeManager::NodeManager() : nodeShader("node"),
                             nodeShadowShader("node_shadow"),
                             nodeConnectorShader("node_connector"),
                             linkShader("link"),
                             basicShader("basic"),
                             boxSelectShader("box_select") {
    setLookAndFeel(std::make_shared<NodeStyle>());
}

NodeManager::~NodeManager() {
    // LINKS FIRST
    for (Link *link: links)
        delete link;
    for (Node *node: nodes)
        delete node;
}

void NodeManager::reset() {
    for (Node *node: nodes)
        node->reset();
    for (Link *link: links)
        link->reset();
}

void NodeManager::addNode(Node *node) {
    nodes.push_back(node);
    doNodeLayout(node);
}

void NodeManager::addLink(Link *link) {
    links.push_back(link);
}

void NodeManager::render(const mat4 &pmat, const mat4 &view, const vec2 &camstart, const vec2 &camend) {
    cullNodes(camstart, camend);
    cullLinks(camstart, camend);
    renderLinks(pmat, view);
    renderShadows(pmat, view);
    //renderNodes(pmat, view);

    glDepthMask(1); // enable depth buffer write
    glColorMask(1,1,1,1); // write to the color buffer
    glDepthFunc(GL_ALWAYS);
    renderNodes(pmat, view);

    // Render Text+connector in batch while culling their pixels with the depth buffer
    glDepthMask(0);
    glColorMask(1,1,1,1);
    glDepthFunc(GL_EQUAL); // texts & connectors must be on the same level as their parent node.
    renderConnectors(pmat, view);
    renderText(pmat, view);
    glDepthFunc(GL_ALWAYS);

    visibleNodes.clear();
    visibleLinks.clear();
}

void NodeManager::beginUpdate() {
#if MULTI_CORES
#pragma omp parallel for
    for (int i = 0; i < nodes.size(); i++)
    {
        nodes[i]->update();
    }
#pragma omp parallel for
    for (int i = 0; i < links.size(); i++)
    {
        links[i]->fetchNextState();
    }
#else
    for (Node *node: nodes)
        node->update();
    for (Link *li: links)
        li->fetchNextState();
#endif
}

void NodeManager::endUpdate() {
#if MULTI_CORES
#pragma omp parallel for
    for (int i = 0; i < links.size(); i++)
    {
        links[i]->propagate();
    }
#else
    for (Link *li: links)
        li->propagate();
#endif
}

void NodeManager::setProgress(float t) {
#if MULTI_CORES
#pragma omp parallel for
    for (int i = 0; i < links.size(); i++)
    {
        links[i]->t = t;
    }
#else
    for (Link *li: links)
        li->t = t;
#endif
}

void NodeManager::removeSelected() {
    for (Node *node: selectedNodes) {
        removeNode(node);
    }
    selectedNodes.clear();
}

std::optional<Node *> NodeManager::getNodeAt(vec2 mouse) {
    for (int i = nodes.size() - 1; i >= 0; --i) {
        Node *node = nodes[i];
        vec2 size = node->size;
        vec2 pos = node->pos;
        if (mouse.x >= pos.x && mouse.x <= pos.x + size.x && mouse.y >= pos.y && mouse.y <= pos.y + size.y)
            return node;
    }
    return {};
}

std::optional<Connector *> NodeManager::getConnectorAt(vec2 mouse) {
    for (int i = nodes.size() - 1; i >= 0; --i) {
        const Node *node = nodes[i];
        vec2 size = node->size;
        vec2 pos = node->pos;
        if (mouse.x >= pos.x && mouse.x <= pos.x + size.x && mouse.y >= pos.y && mouse.y <= pos.y + size.y) {
            // check inputs
            for (Connector *c: node->inputs) {
                vec2 cpos = node->pos + c->pos;
                float r = nodeStyle->connectorRadius;
                if (distanceSq(mouse, cpos) <= r * r)
                    return c;
            }

            // check outputs
            for (Connector *c: node->outputs) {
                vec2 cpos = node->pos + c->pos;
                float r = nodeStyle->connectorRadius;
                if (distanceSq(mouse, cpos) <= r * r)
                    return c;
            }
        }
    }
    return {};
}

void NodeManager::replaceSelected(std::function<Node*(vec2)> build){
    for(Node* node : selectedNodes)
        replaceNode(node, build);
}

void NodeManager::replaceNode(Node* node, std::function<Node*(vec2)> build){
    Node* rnode = build(node->pos);
    // ---- Inputs ----
    for(int i=0; i < node->inputs.size() && i < rnode->inputs.size(); i++){
        Connector* c = node->inputs[i];
        // copy links
        for(Link* l : c->links){
            Link* nl = new Link(l->input, rnode->inputs[i]);
            addLink(nl);
            nl->nextState = l->nextState;
        }
    }
    // ---- Outputs ----
    for(int i=0; i < node->outputs.size() && i <  rnode->outputs.size(); i++){
        Connector* c = node->outputs[i];
        // copy links
        for(Link* l : c->links){
            Link* nl = new Link(rnode->outputs[i], l->output);
            addLink(nl);
        }
    }
    removeNode(node);
    addNode(rnode);
}


void NodeManager::moveSelectedNodes(vec2 offset) {
    for (Node *node: selectedNodes) {
        node->pos = node->pos + offset;
    }
}

void NodeManager::selectNode(Node *node) {
    node->selected = true;
    if (std::find(selectedNodes.begin(), selectedNodes.end(), node) == selectedNodes.end())
        selectedNodes.push_back(node);
}

void NodeManager::boxSelect(vec2 start, vec2 end) {
    vec2 boxStart = vec2(std::min(start.x, end.x), std::min(start.y, end.y));
    vec2 boxEnd = vec2(std::max(start.x, end.x), std::max(start.y, end.y));

    for (Node *node: nodes) {
        vec2 pos = node->pos;
        vec2 size = node->size;
        if (pos.x + size.x >= boxStart.x && pos.x <= boxEnd.x && pos.y + size.y >= boxStart.y && pos.y <= boxEnd.y)
            selectNode(node);
    }
}

void NodeManager::deselectAll() {
    for (Node *node: nodes) {
        node->selected = false;
    }
    selectedNodes.clear();
}

void NodeManager::deselectNode(Node *node) {
    node->selected = false;
    selectedNodes.erase(std::remove(selectedNodes.begin(), selectedNodes.end(), node), selectedNodes.end());
}

bool NodeManager::nodeIsSelected(Node *node) {
    return node->selected;
}


void NodeManager::drawTempLink(Connector *c, vec2 mouse, const mat4 &pmat, const mat4 &view) {
    vec2 start;
    vec2 end;
    if (c->isInput) {
        start = mouse;
        end = c->parent->pos + c->pos;
    } else {
        start = c->parent->pos + c->pos;
        end = mouse;
    }
    float sv[] = {
            start.x, start.y, 1.0f, 0.0f,
            end.x, end.y, 1.0f, 0.0f};
    VertexArray svao;
    VertexBuffer svbo(sv, sizeof(sv));
    VertexBufferLayout slayout;
    slayout.push<float>(2);
    slayout.push<float>(1);
    slayout.push<float>(1);
    svao.addBuffer(svbo, slayout);
    linkShader.use();
    linkShader.setMat4("projection", pmat);
    linkShader.setMat4("view", view);
    linkShader.setFloat("width", nodeStyle->connectorRadius / 2.0f);
    linkShader.setVec3("falseColor", nodeStyle->falseColor);
    linkShader.setVec3("trueColor", nodeStyle->trueColor);
    glDrawArrays(GL_LINES, 0, 2);
}

void NodeManager::drawBoxSelection(vec2 start, vec2 end, const mat4 &pmat, const mat4 &view) {
    vec2 boxStart = vec2(std::min(start.x, end.x), std::min(start.y, end.y));
    vec2 boxEnd = vec2(std::max(start.x, end.x), std::max(start.y, end.y));

    float sv[] = {boxStart.x, boxStart.y, boxEnd.x - boxStart.x, boxEnd.y - boxStart.y};
    VertexArray vao;
    VertexBuffer vbo(sv, sizeof(sv));
    VertexBufferLayout layout;
    layout.push<float>(4); // rect
    vao.addBuffer(vbo, layout);
    boxSelectShader.use();
    boxSelectShader.setMat4("projection", pmat);
    boxSelectShader.setMat4("view", view);
    boxSelectShader.setVec3("selectedColor", nodeStyle->nodeSelectedColor);
    glDrawArrays(GL_POINTS, 0, 1);
}

bool NodeManager::connect(Connector *c1, Connector *c2) {
    if (!c1->isInput && c2->isInput) {
        if (!(c2->links.empty())) {
            Link *li = c2->links[0];
            links.erase(std::remove(links.begin(), links.end(), li), links.end());
            delete li;
        }
        links.push_back(new Link(c1, c2));
        return true;
    } else if (c1->isInput && !c2->isInput) {

        if (!(c1->links.empty())) {
            Link *li = c1->links[0];
            links.erase(std::remove(links.begin(), links.end(), li), links.end());
            delete li;
        }
        links.push_back(new Link(c2, c1));
        return true;
    }
    return false;
}

void NodeManager::disconnectAll(Connector *c) {
    std::vector<Link *> temp = c->links;
    for (Link *li: temp) {
        links.erase(std::remove(links.begin(), links.end(), li), links.end());
        delete li;
    }
}

void NodeManager::removeNode(Node *node) {
    // remove & delete links
    for (Connector *c: node->inputs)
        disconnectAll(c);
    for (Connector *c: node->outputs)
        disconnectAll(c);
    // remove & delete node
    nodes.erase(std::remove(nodes.begin(), nodes.end(), node), nodes.end());
    delete node;
}

void NodeManager::cullNodes(const vec2 &camstart, const vec2 &camend) {
    for (Node *node: nodes) {
        vec2 pos = node->pos - vec2(nodeStyle->shadowSize);
        vec2 size = node->size + vec2(2 * nodeStyle->shadowSize);
        if (pos.x + size.x >= camstart.x && pos.x <= camend.x && pos.y + size.y >= camstart.y && pos.y <= camend.y)
            visibleNodes.push_back(node);
    }
}

void NodeManager::cullLinks(const vec2 &camstart, const vec2 &camend) {
    for (Link *link: links) {
        vec2 start = link->input->parent->pos + link->input->pos;
        vec2 end = link->output->parent->pos + link->output->pos;
        // construct bounding box
        vec2 boxstart = vec2(std::min(start.x, end.x), std::min(start.y, end.y));
        vec2 boxeend = vec2(std::max(start.x, end.x), std::max(start.y, end.y));
        if (boxstart.x <= camend.x && boxstart.y <= camend.y && boxeend.x >= camstart.x && boxeend.y >= camstart.y)
            visibleLinks.push_back(link);
    }
}

void NodeManager::renderShadows(const mat4 &pmat, const mat4 &view) {
    if (visibleNodes.empty())
        return;
    std::vector<float> vertices;
    vertices.reserve(visibleNodes.size() * 5);

    for (const Node *node: visibleNodes) {
        vec2 pos = node->pos - vec2(nodeStyle->shadowSize);
        vec2 size = node->size + vec2(nodeStyle->shadowSize * 2);
        vertices.insert(vertices.end(), {pos.x, pos.y, size.x, size.y, node->selected ? 1.0f : 0.0f});
    }
    VertexArray vao;
    VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
    VertexBufferLayout layout;
    layout.push<float>(4); // rect
    layout.push<float>(1); // selected
    vao.addBuffer(vbo, layout);
    nodeShadowShader.use();
    nodeShadowShader.setMat4("projection", pmat);
    nodeShadowShader.setMat4("view", view);
    nodeShadowShader.setFloat("smoothing", nodeStyle->shadowSize);
    nodeShadowShader.setFloat("radius", nodeStyle->nodeRadius);
    nodeShadowShader.setVec3("selectedColor", nodeStyle->nodeSelectedColor);
    glDrawArrays(GL_POINTS, 0, visibleNodes.size());
}

void NodeManager::renderNodes(const mat4 &pmat, const mat4 &view) {
    if (visibleNodes.empty())
        return;

    std::vector<float> vertices;
    vertices.reserve(visibleNodes.size() * 6);

    for (int i=0; i < visibleNodes.size(); i++) {
        const Node *node = visibleNodes[i];
        vec2 pos = node->pos;
        vec2 size = node->size;
        float headerHeight = node->headerSize.y;
        vertices.insert(vertices.end(), {
            pos.x, pos.y, size.x, size.y,
            headerHeight,
            node->selected ? 1.0f : 0.0f,
            1.0f - (float) i/ (float) visibleNodes.size()});
    }
    VertexArray vao;
    VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
    VertexBufferLayout layout;
    layout.push<float>(4); // rect
    layout.push<float>(1); // headerHeight
    layout.push<float>(1); // selected
    layout.push<float>(1); // depth
    vao.addBuffer(vbo, layout);
    nodeShader.use();
    nodeShader.setMat4("projection", pmat);
    nodeShader.setMat4("view", view);
    nodeShader.setFloat("radius", nodeStyle->nodeRadius);
    nodeShader.setVec3("headerColor", nodeStyle->headerColor);
    nodeShader.setVec3("bodyColor", nodeStyle->bodyColor);
    nodeShader.setVec3("selectedColor", nodeStyle->nodeSelectedColor);
    glDrawArrays(GL_POINTS, 0, visibleNodes.size());
}

void NodeManager::renderText(const mat4 &pmat, const mat4 &view) {
    if (visibleNodes.empty())
        return;
    for (int i=0; i < visibleNodes.size(); i++) {
        const Node *node = visibleNodes[i];
        nodeStyle->font.text(node->name, vec3(node->pos + node->textPos, 1.0f - (float) i / (float) visibleNodes.size()), nodeStyle->headerTextSize,
                             nodeStyle->headerTextColor);
        for (const Connector *c: node->inputs) {
            nodeStyle->font.text(c->name, vec3(c->parent->pos + c->textPos, 1.0f - (float) i / (float) visibleNodes.size()), nodeStyle->connectorTextSize,
                                 nodeStyle->connectorTextColor);
        }
        for (const Connector *c: node->outputs) {
            nodeStyle->font.text(c->name, vec3(c->parent->pos + c->textPos, 1.0f - (float) i / (float) visibleNodes.size()), nodeStyle->connectorTextSize,
                                 nodeStyle->connectorTextColor);
        }
    }
    nodeStyle->font.render(pmat, view);
}

void NodeManager::renderConnectors(const mat4 &pmat, const mat4 &view) {
    if (visibleNodes.empty())
        return;

    std::vector<float> vertices;
    int n = 0;
    for (int i=0; i < visibleNodes.size(); i++) {
        const Node *node = visibleNodes[i];

        for (const Connector *c: node->inputs) {
            vec2 pos = c->parent->pos + c->pos;
            float state = c->state ? 1.0f : 0.0f;
            vertices.insert(vertices.end(), {pos.x, pos.y, nodeStyle->connectorRadius, state, 1.0f - (float) i / (float) visibleNodes.size()});
            n++;
        }
        for (const Connector *c: node->outputs) {
            vec2 pos = c->parent->pos + c->pos;
            float state = c->state ? 1.0f : 0.0f;
            vertices.insert(vertices.end(), {pos.x, pos.y, nodeStyle->connectorRadius, state, 1.0f - (float) i / (float) visibleNodes.size()});
            n++;
        }
    }
    VertexArray vao;
    VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
    VertexBufferLayout layout;
    layout.push<float>(4); // (x,y,r,state)
    layout.push<float>(1); // depth
    vao.addBuffer(vbo, layout);
    nodeConnectorShader.use();
    nodeConnectorShader.setMat4("projection", pmat);
    nodeConnectorShader.setMat4("view", view);
    nodeConnectorShader.setVec3("falseColor", nodeStyle->falseColor);
    nodeConnectorShader.setVec3("trueColor", nodeStyle->trueColor);
    glDrawArrays(GL_POINTS, 0, n);
}

void NodeManager::renderLinks(const mat4 &pmat, const mat4 &view) {
    if (visibleLinks.empty())
        return;
    std::vector<float> vertices;
    vertices.reserve(visibleLinks.size() * 8);

    for (const Link *li: visibleLinks) {
        vec2 inPos = li->input->parent->pos + li->input->pos;
        vec2 outPos = li->output->parent->pos + li->output->pos;

        float startTrue = li->state == false && li->nextState == true || li->state == true && li->nextState == true;
        float t = li->state == false && li->nextState == true || li->state == true && li->nextState == false ? li->t
                                                                                                             : 1.0;

        vertices.insert(vertices.end(), {inPos.x, inPos.y, t, startTrue,
                                         outPos.x, outPos.y, t, startTrue});
    }
    VertexArray vao;
    VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
    VertexBufferLayout layout;
    layout.push<float>(2); // pos
    layout.push<float>(1); // t
    layout.push<float>(1); // startTrue
    vao.addBuffer(vbo, layout);

    linkShader.use();
    linkShader.setMat4("projection", pmat);
    linkShader.setMat4("view", view);
    linkShader.setFloat("width", nodeStyle->connectorRadius / 2.0f);
    linkShader.setVec3("falseColor", nodeStyle->falseColor);
    linkShader.setVec3("trueColor", nodeStyle->trueColor);
    glDrawArrays(GL_LINES, 0, 2 * visibleLinks.size());
}

void NodeManager::doNodeLayout(Node *node) {
    const float headerHeight = nodeStyle->font.getHeight(node->name, nodeStyle->headerTextSize) + nodeStyle->nodeRadius;
    const float headerWidth =
            nodeStyle->font.getWidth(node->name, nodeStyle->headerTextSize) + 2 * nodeStyle->nodeRadius;
    node->textPos = vec2(nodeStyle->nodeRadius, nodeStyle->nodeRadius);
    vec2 inputsSize = getConnectorSize(node, false);
    vec2 outputsSize = getConnectorSize(node, true);
    node->size = vec2(std::max(headerWidth, inputsSize.x + nodeStyle->inOutDist + outputsSize.x),
                      headerHeight + std::max(inputsSize.y, outputsSize.y));

    node->headerSize = vec2(node->size.x, headerHeight);
    doConnectorLayout(node, false);
    doConnectorLayout(node, true);
}

vec2 NodeManager::getConnectorSize(Node *node, bool output) {
    vec2 connectorSize = vec2(0);
    std::vector<Connector *> &connectors = output ? node->outputs : node->inputs;
    for (Connector *c: connectors) {
        connectorSize.y += nodeStyle->connectorMargin.y * 2 + std::max(nodeStyle->connectorRadius * 2,
                                                                       nodeStyle->font.getHeight(c->name,
                                                                                                 nodeStyle->connectorTextSize));
        connectorSize.x = std::max(connectorSize.x, nodeStyle->connectorMargin.x + nodeStyle->connectorRadius * 2 +
                                                    nodeStyle->font.getWidth(c->name, nodeStyle->connectorTextSize) +
                                                    nodeStyle->textOutletMargin);
    }
    return connectorSize;
}

void NodeManager::doConnectorLayout(Node *node, bool output) {
    float inOffsetY = node->headerSize.y;
    std::vector<Connector *> &connectors = output ? node->outputs : node->inputs;
    for (Connector *c: connectors) {
        inOffsetY += nodeStyle->connectorMargin.y;
        vec2 outletPos;
        vec2 connectorTextPos;
        if (output) {
            float textWidth = nodeStyle->font.getWidth(c->name, nodeStyle->connectorTextSize);
            outletPos.x = node->size.x - nodeStyle->connectorMargin.x - nodeStyle->connectorRadius;
            connectorTextPos.x =
                    node->size.x - nodeStyle->connectorMargin.x - 2 * nodeStyle->connectorRadius - textWidth -
                    nodeStyle->textOutletMargin;
        } else {
            outletPos.x = nodeStyle->connectorMargin.x + nodeStyle->connectorRadius;
            connectorTextPos.x =
                    nodeStyle->connectorMargin.x + 2 * nodeStyle->connectorRadius + nodeStyle->textOutletMargin;
        }
        float textHeight = nodeStyle->font.getHeight(c->name, nodeStyle->connectorTextSize);
        float totalHeight = textHeight;
        if (nodeStyle->connectorRadius * 2 <= textHeight) {
            connectorTextPos.y = inOffsetY;
            outletPos.y = inOffsetY + textHeight / 2.0f;
        } else {
            outletPos.y = inOffsetY + nodeStyle->connectorRadius;
            connectorTextPos.y = inOffsetY + nodeStyle->connectorRadius - textHeight / 2.0f;
            totalHeight = nodeStyle->connectorRadius * 2;
        }
        // update connector pos
        c->pos = outletPos;
        c->textPos = connectorTextPos;
        inOffsetY += totalHeight;
    }
}

void NodeManager::setLookAndFeel(std::shared_ptr<NodeStyle> style) {
    nodeStyle = style;
    for (Node *node: nodes)
        doNodeLayout(node);
}