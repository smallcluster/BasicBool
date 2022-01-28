#pragma once
#include "node_system.hpp"

class TrueNode : public Node
{
public:
    TrueNode(vec2 pos) : Node("TRUE", pos)
    {
        addOutput("out");
        outputs[0]->state = true;
    }
    void update() override
    {
    }
    void reset() override {
    }
};

class NotNode : public Node
{
public:
    NotNode(vec2 pos) : Node("NOT", pos)
    {
        addInput("in");
        addOutput("out");
        outputs[0]->state = true;
    }
    void update() override
    {
        outputs[0]->state = !(inputs[0]->state);
    }
    void reset() override {
        outputs[0]->state = true;
        inputs[0]->state = false;
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
        outputs[0]->state = inputs[0]->state && inputs[1]->state;
    }
    void reset() override {
        outputs[0]->state = false;
        inputs[0]->state = false;
        inputs[1]->state = false;
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
        outputs[0]->state = inputs[0]->state || inputs[1]->state;
    }
    void reset() override {
        outputs[0]->state = false;
        inputs[0]->state = false;
        inputs[1]->state = false;
    }
};