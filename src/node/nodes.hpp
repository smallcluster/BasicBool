#pragma once
#include "node_system.hpp"

class TrueNode : public Node
{
public:
    TrueNode(vec2 pos, NodeStyle &style) : Node("TRUE", pos, style)
    {
        addOutput("out");
        getOutput("out")->state = true;
    }
    TrueNode(vec2 pos) : Node("TRUE", pos)
    {
        addOutput("out");
        getOutput("out")->state = true;
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
        getOutput("out")->state = true;
    }
    NotNode(vec2 pos) : Node("NOT", pos)
    {
        addInput("in");
        addOutput("out");
        getOutput("out")->state = true;
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