#pragma once

#include "core/math.hpp"
#include "platform/platform.hpp"
#include <optional>
#include <vector>
#include <functional>


namespace gui {

    class Widget;

    class Layout {
    public:
        Layout(){}
        virtual void doLayout(std::vector<Widget *> &widgets, vec2 pos, vec2 size){};
    };

    class Widget {
    public:
        Widget(std::optional<Widget *> parent) : parent(parent) {}

        vec2 getScreenPos() {
            return pos + (parent ? parent.value()->getScreenPos() : vec2(0));
        }

        bool isVisible() {
            return visible;
        }

        virtual void draw(){};
        virtual void doLayout(){}

    protected:
        std::optional<Widget *> parent;
        vec2 pos;
        vec2 size;
        bool visible = true;
        Layout layout;
        vec2 minSize;
    };

    class CompositeWidget : public Widget {
    public:
        CompositeWidget(std::optional<Widget *> parent) : Widget(parent) {}

        void draw() override {
            for (auto &w: childs) {
                if (w->isVisible())
                    w->draw();
            }
        }

        void doLayout() override {
            layout.doLayout(childs, pos, size);
            for (auto &w: childs) {
                if (w->isVisible())
                    w->doLayout();
            }
        }

    private:
        std::vector<Widget *> childs;
    };

} // namespace gui