#pragma once

#include "gui.hpp"

namespace gui {

    struct UnitConstraint {
        enum Type {
            PIXEL,
            PERCENT,
            DP
        };
        Type type;
        float value;

        UnitConstraint(Type type, float value) : type(type), value(value) {}
    };

    struct SizeConstraint {
        enum Type {
            WARP_CONTENT,
            EXPEND,
            UNIT
        };
        Type type;
        std::optional<UnitConstraint> unitConstraint = {};

        SizeConstraint(UnitConstraint c) : type(Type::UNIT), unitConstraint{c} {}

        SizeConstraint(Type type) : type(type) {}
    };

    struct PositionConstraint {
        enum Side {
            LEFT,
            RIGHT,
            TOP,
            BOTTOM
        };
        Side side;
        std::optional<Widget *> widget = {};

        PositionConstraint(Side side, Widget *widget) : side(side), widget{widget} {}

        PositionConstraint(Side side) : side(side) {}
    };


    struct Constraints {
        std::optional<SizeConstraint> width = SizeConstraint(SizeConstraint::WARP_CONTENT);
        std::optional<SizeConstraint> height = SizeConstraint(SizeConstraint::WARP_CONTENT);
        std::optional<PositionConstraint> leftTo = PositionConstraint(PositionConstraint::LEFT);
        std::optional<PositionConstraint> topTo = PositionConstraint(PositionConstraint::TOP);
        std::optional<PositionConstraint> rightTo = {};
        std::optional<PositionConstraint> bottomTo = {};
        std::optional<UnitConstraint> leftMargin = {};
        std::optional<UnitConstraint> rightMargin = {};
        std::optional<UnitConstraint> topMargin = {};
        std::optional<UnitConstraint> bottomMargin = {};
    };


    class ConstraintLayout : public Layout {
    public:
        void add(Widget *widget, Constraints c) {
            widgets.emplace_back(widget, c);
        }

    private:
        std::vector<std::pair<Widget *, Constraints>> widgets;
    };
}