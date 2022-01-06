#pragma once
#include "core/defines.hpp"
#include "core/math.hpp"
#include <glad/glad.h>
#include <vector>

// See below
class Connector;

class Node
{
private:
    float m_width;
    float m_height;
    float m_headerHeight;
    vec2 m_pos;
    string m_name;
    std::vector<Connector> m_inputs;
    std::vector<Connector> m_outputs;
    void updateSize();
public:
    Node(const string &name, const vec2 &pos);
    Node(const string &name, const vec2 &pos, std::vector<Connector> inputs, std::vector<Connector> outputs);
    ~Node();
    void draw();
};


class Connector
{
    private:
        vec2 m_pos; // for rendering
        string m_name;
        std::vector<Connector*> m_links;
        Node &m_parent; // parent node
        bool m_updated; // to check if this connector was updated
        bool m_signalOn; // True -> green drawing

    public:
        Connector();
        ~Connector();
        void draw();
};

