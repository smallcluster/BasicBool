#pragma once
#include "backend.hpp"
#include "core/math.hpp"
#include <map>
#include <fstream>

// TODO : All
class ParserFnt{
    private:
        std::fstream m_file;
    public:
        ParserFnt(const string & path);
        bool nextLine(std::pair<string, std::map<string, string>> &line); // <type, params->value>
};

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
        float m_size;
        float m_lineHeight;
        vec2 m_textureDim;
        vec4 m_padding;
        std::map<char, Glyph> m_glyphs;
    public:
        Font(const string &name);
        Texture &getTexture();
};

struct Text {
    string textString;
    float fontSize;
    vec3 color;
    vec2 position;
};

class TextRenderer{
    protected:
        TextRenderer(TextRenderer &other);
        ~TextRenderer();
        TextRenderer();
        void operator=(const TextRenderer & other);

    private:
        std::map<Font, Text> m_data;
    public:
        static TextRenderer &getInstance();
        void text(const string &textString, vec2 position, float size, Font font);
        void render();
};