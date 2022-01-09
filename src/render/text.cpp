#include "text.hpp"
#include <fstream>
#include <map>


ParserFnt::ParserFnt(const string &path) : m_file(path){}

std::vector<string> split(const string &s, char c){
    std::vector<string> data;
    std::stringstream buffer;
    int i = 0;
    while( i < s.length()){
        // ignore sequence of separator
        while( i < s.length() && s[i] == c)
            i++;
        // no other char
        if(i == s.length())
            break;

        // read chunk
        while(i < s.length() && s[i] != c){
            buffer << s[i];
            i++;
        }
        // push chunk
        data.push_back(buffer.str());
        buffer.str(string()); // clear buffer
    }
    return data;
}

bool ParserFnt::nextLine(std::pair<string, std::map<string, string>> &line){
    string s;
    if(std::getline(m_file, s)){
        // read all attributes
        std::vector<string> params = split(s, ' ');
        line.first = params[0];
        for(int i=1; i < params.size(); i++){
            std::vector<string> val = split(params[i], '=');
            line.second[val[0]] = val[1];
        }
        return true;
    }
    return false;
}


Font::Font(const string& name) : m_texture("res/fonts/"+name+".png") {
    string path = "res/fonts/"+name+".fnt";
    LOGDEBUG("loading font : {}", path);

    ParserFnt parser(path);

    // Lecture ligne par ligne
    std::pair<string, std::map<string, string>> line;
    while(parser.nextLine(line)){
        if(line.first == "info"){
            m_name = line.second["face"];

            m_size = std::stof(line.second["size"]);

            auto pvals = split(line.second["padding"], ',');
            m_padding.x = std::stof(pvals[0]);
            m_padding.y = std::stof(pvals[1]);
            m_padding.z = std::stof(pvals[2]);
            m_padding.w = std::stof(pvals[3]);

        } else if(line.first == "common"){
            m_lineHeight = std::stof(line.second["lineHeight"]);

            m_textureDim.x = std::stof(line.second["scaleW"]);
            m_textureDim.y = std::stof(line.second["scaleH"]);

        } else if(line.first == "char"){
            Glyph glyph;
            glyph.id = std::stoi(line.second["id"]);

            glyph.advance = std::stof(line.second["xadvance"]);

            glyph.dim.x = std::stof(line.second["width"]);
            glyph.dim.y = std::stof(line.second["height"]);

            glyph.pos.x = std::stof(line.second["x"]);
            glyph.pos.y = std::stof(line.second["y"]);

            glyph.offset.x = std::stof(line.second["xoffset"]);
            glyph.offset.y = std::stof(line.second["yoffset"]);

        } else if(line.first == "kernings"){
            break;
        }
    }
}