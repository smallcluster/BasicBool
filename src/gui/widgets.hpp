#pragma once
#include "gui.hpp"

namespace gui {

    class Label : public Widget {
    public:
        Label(std::optional<Widget*> parent, string text) : Widget(parent), text(text) {}
    private:
        string text;
    };

    class Button : public Widget{
    public:
        Button(std::optional<Widget*> parent, string text) : Widget(parent), text(text) {}
    private:
        string text;
    };

    class Slider : public Widget{
    public:
        Slider(std::optional<Widget*> parent, string text, vec2 range) : Widget(parent), text(text), range(range) {}
    private:
        string text;
        vec2 range;
    };

    class InputText : public Widget {
    public:
        InputText(std::optional<Widget*> parent, string text) : Widget(parent), text(text){}
    private:
        string text;
    };

    // ---- Composite widgets ----

}