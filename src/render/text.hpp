#pragma once
#include "backend.hpp"
#include "core/math.hpp"
#include <map>
#include <fstream>
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

struct Text {
    string textString;
    float fontSize;
    vec2 position;
    vec3 color;
};
class Font {
    private:
        string m_name;
        Texture m_texture;
        float m_size;
        float m_lineHeight;
        vec4 m_padding;
        std::map<char, Glyph> m_glyphs;
        std::vector<Text> m_texts;
    public:
        Font(const string &name);
        Texture &getTexture();
        std::map<char, Glyph> &getGlyphs();
        string getName();
        float getSize();
        float getLigneHeight();
        vec4 getPadding();
        void text(const string &textString, vec2 position, float size, vec3 color);
        void render(Shader &shader, const mat4 &projection);
};