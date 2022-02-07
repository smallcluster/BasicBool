#pragma once

#include "core/math.hpp"
#include "platform/platform.hpp"
#include "render/backend.hpp"
#include "render/text.hpp"
#include <optional>
#include <vector>
#include <algorithm>

namespace gui {

    struct Theme {
        float cornerRadius = 8.0f;
        float dropDownTextSize = 14;
        float dropDownMargin = 4.0f;
        vec3 backgoundColor = vec3(0.1f);
        vec3 textColor = vec3(1.0f);
        vec3 dropDownHoverColor = vec3(1, 0.5f, 0);
        vec3 dropDownColor = vec3(0.2f);
        Font &font;

        Theme(Font &font) : font(font) {};
        Theme() : font(Font::getDefault()) {};
    };

    struct Rect {
        vec2 pos;
        vec2 size;
        vec3 color = vec3(0);

        Rect(){}
        Rect(vec2 pos, vec2 size) : pos(pos), size(size) {}
        Rect(vec2 pos, vec2 size, vec3 color) : pos(pos), size(size), color(color) {}

        static Rect findMinAABB(Rect a, Rect b) {
            vec2 p = vec2(std::min(a.pos.x, b.pos.x), std::min(a.pos.y, b.pos.y));
            vec2 s = vec2(std::max(a.pos.x + a.size.x, b.pos.x + b.size.x) - p.x,
                          std::max(a.pos.y + a.size.y, b.pos.y + b.size.y) - p.y);
            return {p,s};
        }
        static bool pointInRect(Rect r, vec2 p) {
            return (p.x >= r.pos.x && p.y >= r.pos.y && p.x <= r.pos.x + r.size.x && p.y <= r.pos.y + r.size.y);
        }
    };


    class GUIManager {
    public:

        GUIManager() : guiRectShader("gui_rect"){}

        // TODO : Improve this
        int dropDownMenu(const std::vector<string> &list, vec2 pos, const mat4 &pmat, std::optional<string> header) {
            if (list.empty())
                return -1;

            // User interaction
            Platform &platform = Platform::getInstance();
            vec2 mouse = vec2(platform.getMouseX(), platform.getMouseY());

            // Find dropDowm size

            Rect minAABB;
            int startIndex;

            vec2 offset;
            if(header){
                startIndex = 0;
                minAABB = Rect(pos, theme.font.getDim(header.value(), theme.dropDownTextSize)+vec2(2*theme.cornerRadius));

                theme.font.text(header.value(), pos+vec2(theme.cornerRadius), theme.dropDownTextSize, theme.textColor);
                offset = vec2(0, minAABB.size.y+theme.dropDownMargin);
            } else {
                startIndex = 1;
                minAABB = Rect(pos, theme.font.getDim(list[0], theme.dropDownTextSize)+vec2(2*theme.cornerRadius));
                offset = vec2(0, minAABB.size.y);
            }


            for (int i = startIndex; i < list.size(); i++) {
                Rect textRect = Rect(pos + offset, theme.font.getDim(list[0], theme.dropDownTextSize)+vec2(2*theme.cornerRadius));
                minAABB = Rect::findMinAABB(minAABB, textRect);
                offset.y += textRect.size.y + theme.dropDownMargin;
            }

            // draw bg rectangle
            drawRect(Rect(
                    minAABB.pos-vec2(theme.dropDownMargin),
                    minAABB.size+vec2(2*theme.dropDownMargin, 3*theme.dropDownMargin),
                    theme.backgoundColor));

            // render each item && check which one is selected
            int selected = -1;

            if(header)
                offset = vec2(0, theme.font.getHeight(header.value(), theme.dropDownTextSize)+ 2*theme.dropDownMargin+2*theme.cornerRadius);
            else
                offset = vec2(0);

            for (int i = 0; i < list.size(); i++) {
                float txtHeight = theme.font.getHeight(list[0], theme.dropDownTextSize);
                Rect r = Rect(pos+offset, vec2(minAABB.size.x, txtHeight+theme.cornerRadius*2));

                vec3 bgColor = theme.dropDownColor;
                if (Rect::pointInRect(r, mouse)) {
                    bgColor = theme.dropDownHoverColor;
                    if (Platform::getInstance().isMousePressed(MouseButton::LEFT))
                        selected = i;
                }
                r.color = bgColor;
                drawRect(r);
                theme.font.text(list[i], r.pos+vec2(theme.cornerRadius), theme.dropDownTextSize, theme.textColor);
                offset.y += r.size.y + theme.dropDownMargin;
            }

            // render widget
            renderRect(pmat);
            theme.font.render(pmat);
            return selected;
        }

    private:
        Theme theme;
        Shader guiRectShader;
        std::vector<Rect> rectDrawCalls;

        void drawRect(Rect r){
            rectDrawCalls.push_back(r);
        }

        void renderRect(const mat4 &pmat){
            std::vector<float> verts;
            verts.reserve(rectDrawCalls.size()*7);
            for(auto &r : rectDrawCalls)
                verts.insert(verts.end(),{r.pos.x, r.pos.y,r.size.x, r.size.y, r.color.r, r.color.g, r.color.b});
            VertexArray vao;
            VertexBuffer vbo(&verts[0], sizeof(float) * verts.size());
            VertexBufferLayout layout;
            layout.push<float>(4); // rect
            layout.push<float>(3); // color
            vao.addBuffer(vbo, layout);
            guiRectShader.use();
            guiRectShader.setMat4("view", pmat);
            guiRectShader.setFloat("radius", theme.cornerRadius);
            glDrawArrays(GL_POINTS, 0, rectDrawCalls.size());

            rectDrawCalls.clear();
        }

    };

}