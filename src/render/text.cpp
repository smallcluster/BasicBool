#include "text.hpp"
#include <fstream>
#include <map>
#include <algorithm>

// ---- PARSERFNT ----

ParserFnt::ParserFnt(const string &path) : m_file(path) {}

std::vector<string> split(const string &s, char c) {
    std::vector<string> data;
    std::stringstream buffer;
    int i = 0;
    while (i < s.length()) {
        // ignore sequence of separator
        while (i < s.length() && s[i] == c)
            i++;

        // no other char
        if (i == s.length())
            break;

        // read chunk
        // special check for "  " -> must read the separator
        bool isString = false;
        while (i < s.length()) {
            if (s[i] == '"' && !isString)
                isString = true;
            else if (s[i] == '"' && isString)
                isString = false;

            if (s[i] != c || (s[i] == c && isString)) {
                buffer << s[i];
                i++;
            } else
                break;
        }

        // push chunk
        string chunk = buffer.str();
        data.push_back(chunk);
        buffer.str(string()); // clear buffer
    }
    return data;
}

bool ParserFnt::nextLine(std::pair<string, std::map<string, string>> &line) {
    string s;
    if (std::getline(m_file, s)) {
        // read all attributes
        std::vector<string> params = split(s, ' ');

        if (params.empty())
            return false;

        line.first = params[0];

        for (int i = 1; i < params.size(); i++) {
            std::vector<string> val = split(params[i], '=');
            line.second[val[0]] = val[1];
        }
        return true;
    }
    return false;
}

// ---- FONT ---

Font::Font(const string &name) : m_texture("res/fonts/" + name + ".png"), textShader("text_sdf") {
    string path = "res/fonts/" + name + ".fnt";
    LOGDEBUG("loading font : {}", path);
    ParserFnt parser(path);
    // Lecture ligne par ligne
    std::pair<string, std::map<string, string>> line;
    while (parser.nextLine(line)) {
        if (line.first == "info") {
            m_name = line.second["face"];
            m_size = std::stof(line.second["size"]);
            auto pvals = split(line.second["padding"], ',');
            m_padding.x = std::stof(pvals[0]);
            m_padding.y = std::stof(pvals[1]);
            m_padding.z = std::stof(pvals[2]);
            m_padding.w = std::stof(pvals[3]);
        } else if (line.first == "common") {
            m_lineHeight = std::stof(line.second["lineHeight"]);
        } else if (line.first == "char") {
            Glyph glyph;
            glyph.id = std::stoi(line.second["id"]);

            glyph.advance = std::stof(line.second["xadvance"]);

            glyph.dim.x = std::stof(line.second["width"]);
            glyph.dim.y = std::stof(line.second["height"]);

            glyph.pos.x = std::stof(line.second["x"]);
            glyph.pos.y = std::stof(line.second["y"]);

            glyph.offset.x = std::stof(line.second["xoffset"]);
            glyph.offset.y = std::stof(line.second["yoffset"]);

            m_glyphs[glyph.id] = glyph; // store glyph info
        } else if (line.first == "kernings") {
            break;
        }
    }
}

std::map<char, Glyph> &Font::getGlyphs() { return m_glyphs; }

// TODO : improve metrics calculations
float Font::getHeight(const string &text, float size) {
    float scale = size / m_size;
    float lineHeight = m_lineHeight * scale;
    float height = lineHeight;
    for (char c: text)
        if (c == '\n')
            height += lineHeight;
    return height;
}

float Font::getWidth(const string &text, float size) {
    float scale = size / m_size;
    float x = 0;
    float lastAdvance = 0;
    float lastWidth = 0;
    std::vector<float> widths;
    for (char c: text) {
        Glyph g;
        if (c == '\n') {
            widths.push_back(x);
            x = 0;
            continue;
        }

        if (m_glyphs.count(c) == 0)
            g = m_glyphs[(char) 127];
        else
            g = m_glyphs[c];

        lastAdvance = (g.advance-m_padding.x) * scale;
        lastWidth = g.dim.x * scale;
        x += lastAdvance;
    }
    widths.push_back(x - lastAdvance + lastWidth);
    float max = widths[0];
    for (float w: widths)
        if (w > max)
            max = w;

    return max;
}

vec2 Font::getDim(const string &text, float size){
    return {getWidth(text, size), getHeight(text, size)};
}

void Font::text(const string &textString, vec2 position, float size, vec3 color) {
    Text t;
    t.fontSize = size;
    t.position = vec3(position, 0);
    t.textString = textString;
    t.color = color;
    m_texts.push_back(t);
}
void Font::text(const string &textString, vec3 position, float size, vec3 color) {
    Text t;
    t.fontSize = size;
    t.position = position;
    t.textString = textString;
    t.color = color;
    m_texts.push_back(t);
}

/*
void Font::render(const mat4 &pmat, const mat4 &view)
{
    // Nothing to render
    if (m_texts.empty())
        return;

    // build geometry
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    vec2 texDim = vec2(m_texture.getWidth(), m_texture.getHeight());

    unsigned int nbChars = 0;
    for (const Text &txt : m_texts)
    {

        // Text infos
        float scale = txt.fontSize / m_size;

        string s = txt.textString;
        vec3 color = txt.color;

        vec2 pos = txt.position; // writer head

        float xstart = pos.x;
        for (std::string::size_type j = 0; j < s.size(); j++)
        {
            char c = s[j];
            // check if char is in supported charset
            // else default to unknown char
            Glyph g;

            // new line
            if (c == '\n')
            {
                pos.x = xstart;
                pos.y += m_lineHeight * scale;
                continue;
            }

            if (m_glyphs.count(c) == 0)
                g = m_glyphs[(char)127];
            else
                g = m_glyphs[c];

            // build quad for this char
            // pos
            vec2 p0 = pos + (g.offset - m_padding.xy) * scale;
            vec2 p1 = p0 + vec2(g.dim.x + 2 * m_padding.x, 0) * scale;
            vec2 p2 = p0 + (vec2(g.dim.x, g.dim.y) + 2 * m_padding.xy) * scale;
            vec2 p3 = p0 + vec2(0, g.dim.y + 2 * m_padding.y) * scale;

            // uvs
            vec2 u0 = vec2(g.pos.x, g.pos.y);
            vec2 u1 = (u0 + vec2(g.dim.x, 0)) / texDim;
            vec2 u2 = (u0 + vec2(g.dim.x, g.dim.y)) / texDim;
            vec2 u3 = (u0 + vec2(0, g.dim.y)) / texDim;
            u0 = u0 / texDim;

            float rectVertices[] = {
                p0.x,
                p0.y,
                u0.x,
                u0.y,
                color.r,
                color.g,
                color.b,
                p1.x,
                p1.y,
                u1.x,
                u1.y,
                color.r,
                color.g,
                color.b,
                p2.x,
                p2.y,
                u2.x,
                u2.y,
                color.r,
                color.g,
                color.b,
                p3.x,
                p3.y,
                u3.x,
                u3.y,
                color.r,
                color.g,
                color.b,
            };
            unsigned int offset = nbChars * 4;
            unsigned int rectIndices[] = {
                offset, offset + 1, offset + 2, offset, offset + 2, offset + 3};

            // append data
            vertices.insert(vertices.end(), std::begin(rectVertices), std::end(rectVertices));
            indices.insert(indices.end(), std::begin(rectIndices), std::end(rectIndices));

            // advance writer head
            pos.x += g.advance * scale;

            // count nb of chars
            nbChars++;
        }
    }
    textShader.use();
    textShader.setMat4("projection", pmat);
    textShader.setMat4("view", view);

    m_texture.bind(0);
    VertexArray vao;
    VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
    ElementBuffer ebo(&indices[0], sizeof(unsigned int) * indices.size());
    VertexBufferLayout layout;
    layout.push<float>(2); // pos
    layout.push<float>(2); // uv
    layout.push<float>(3); // color
    vao.addBuffer(vbo, layout);
    vao.addBuffer(ebo);

    glDrawElements(GL_TRIANGLES, nbChars * 6, GL_UNSIGNED_INT, 0);

    m_texts.clear(); // remove all draw calls
}
*/

void Font::render(const mat4 &pmat, const mat4 &view) {
    // Nothing to render
    if (m_texts.empty())
        return;

    // get total vertices
    unsigned int nbMaxChars = 0;
    for (const Text &txt: m_texts)
        nbMaxChars += txt.textString.size();

    // build geometry
    std::vector<float> vertices;
    vertices.reserve(nbMaxChars * 11);
    vec2 texDim = vec2(m_texture.getWidth(), m_texture.getHeight());

    unsigned int nbChars = 0;
    for (const Text &txt: m_texts) {
        // Text infos
        float scale = txt.fontSize / m_size;
        const string &s = txt.textString;
        vec3 color = txt.color;
        vec3 pos = txt.position; // writer head

        float xstart = pos.x;
        for (int j = 0; j < s.length(); j++) {
            char c = s[j];
            // check if char is in supported charset
            // else default to unknown char
            Glyph g;

            // new line
            if (c == '\n') {
                pos.x = xstart;
                pos.y += m_lineHeight * scale;
                continue;
            }

            if (m_glyphs.count(c) == 0)
                g = m_glyphs[(char) 127];
            else
                g = m_glyphs[c];

            // build quad for this char
            // pos
            //vec2 start = pos + (g.offset - m_padding.xy) * scale;
            vec2 start = pos.xy + g.offset * scale;
            vec2 size = vec2(g.dim.x, g.dim.y) * scale;

            // uvs
            vec2 uvStart = vec2(g.pos.x, g.pos.y) / texDim;
            vec2 uvSize = vec2(g.dim.x, g.dim.y) / texDim;

            // append data
            vertices.insert(vertices.end(), {start.x, start.y, size.x, size.y,
                                             uvStart.x, uvStart.y, uvSize.x, uvSize.y,
                                             color.r, color.g, color.b,
                                             pos.z});

            // advance writer head
            pos.x += (g.advance-m_padding.x) * scale;
            nbChars++;
        }
    }
    textShader.use();
    textShader.setMat4("projection", pmat);
    textShader.setMat4("view", view);
    m_texture.bind(0);
    VertexArray vao;
    VertexBuffer vbo(&vertices[0], sizeof(float) * vertices.size());
    VertexBufferLayout layout;
    layout.push<float>(4); // rect
    layout.push<float>(4); // rectUV
    layout.push<float>(3); // color
    layout.push<float>(1); // depth
    vao.addBuffer(vbo, layout);

    glDrawArrays(GL_POINTS, 0, nbChars);

    m_texts.clear(); // remove all draw calls
}
