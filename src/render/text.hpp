#pragma once
#include "backend.hpp"
#include "core/math.hpp"
#include <map>

// TODO : All

struct Glyph {
    char id;
    vec2 pos;
    vec2 dim;
    vec2 offset;
    float advance;
};

class Font {
    private:
        string m_name;
        Texture m_texture;
        float m_ascent, m_descent;
        std::map<char, Glyph> m_glyphs;
    public:
        Font(const string &path);
        ~Font();
        float getHeight();
        float getWidth(const string &textString);
};

class Text {
    private:
        // Text settings
        string m_textString;
        float m_fontSize;
        vec3 m_color;
        Font m_font;
        vec2 m_position;

        // OpenGL stuff
        VertexArray m_vao;
        VertexBuffer m_vbo;
        ElementBuffer m_ebo;

    public:
        Text(Font font, const string &textString, vec2 position);
        void draw();
        ~Text();
};


class TextRenderer{
    private:
        static std::map<Font, Text> m_data;
    public:
        static void text(const string &textString, vec2 position, float size, Font font);
        static void render();
};