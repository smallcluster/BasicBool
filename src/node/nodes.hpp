#pragma once
#include "core/defines.hpp"
#include "core/math.hpp"
#include <unordered_map>
#include "render/text.hpp"



struct Connector{
    bool state = false;
    string name;
    Connector(string name) : name(name) {}

    // TODO : Draw the connector
    void draw(Font &font);
};


// in -> out
struct Link {
    bool state = false;
    Connector* input;
    Connector* output;
    Link(Connector *in, Connector *out) : input(in), output(out){}
    ~Link(){
        output->state = false;
    }
    void update(){
        output->state = input->state;
    }

    void reset(){
        input->state = false;
        output->state = false;
    }

    // TODO : Draw the link
    void draw();
};


class Node {
    private:
        std::unordered_map<string, Connector*> inputs;
        std::unordered_map<string, Connector*> outputs;
    public:
        string name;
        vec2 pos;

        // TODO : Draw the node
        void draw(Font &font);

        Node(string name) : name(name) {}
        ~Node(){
            auto itInputs = inputs.begin();
            while (itInputs != inputs.end())
            {
                delete itInputs->second;
                itInputs++;
            }
            auto itOutputs = outputs.begin();
            while (itOutputs != outputs.end())
            {
                delete itOutputs->second;
                itOutputs++;
            }
        }

        bool addOutput(string name){
            if(outputs.find(name) != outputs.end()){
                outputs[name] = new Connector(name);
                return true;
            }
            return false;
        }
        bool addInput(string name){
            if(inputs.find(name) != inputs.end()){
                inputs[name] = new Connector(name);
                return true;
            }
            return false;
        }
        Connector* getInput(string name){
            auto it = inputs.find(name);
            if(it != inputs.end())
                return it->second;
            return nullptr;
        }
        Connector* getOutput(string name){
            auto it = outputs.find(name);
            if(it != outputs.end())
                return it->second;
            return nullptr;
        }

        virtual void update();
};

// ------ Basic nodes ---------------

class TrueNode : public Node {
    public:
        TrueNode() : Node("TRUE") {
            addOutput("out");
        }
        void update() override {
            getOutput("out")->state = true;
        }
};

class NotNode : public Node {
    public:
        NotNode() : Node("NOT"){
            addInput("in");
            addOutput("out");
        }
        void update() override {
            getOutput("out")->state = !(getInput("in")->state);
        }
};

class AndNode : public Node {
    public:
        AndNode() : Node("AND") {
            addInput("in1");
            addInput("in2");
            addOutput("out");
        }
        void update() override {
            getOutput("out")->state = getInput("in1")->state && getInput("in2")->state; 
        }
};

class OrNode : public Node {
    public:
        OrNode() : Node("OR") {
            addInput("in1");
            addInput("in2");
            addOutput("out");
        }
        void update() override {
            getOutput("out")->state = getInput("in1")->state || getInput("in2")->state; 
        }
};

// ---- Simulation ----
class NodeManager {
    private:
        std::vector<Node*> nodes;
        std::vector<Link*> links;
    public:
        bool addNode(string name){
            if(name == "TRUE"){
                nodes.push_back(new TrueNode());
                return true;
            } else if(name == "NOT"){
                nodes.push_back(new NotNode());
                return true;
            } else if(name == "AND"){
                nodes.push_back(new AndNode());
                return true;
            } else if(name == "OR"){
                nodes.push_back(new OrNode());
                return true;
            }
            return false;
        }

        void draw(Font& font){
            for(Node* n : nodes)
                n->draw(font);
            for(Link* li : links)
                li->draw();
        }

        void simulate(){
            // Nodes THEN links
            for(Node* n : nodes)
                n->update();
            for(Link* li : links)
                li->update();
        }
};
