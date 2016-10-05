#pragma once
#include "frame/Component.h"
#include "frame_gl/math.h"

namespace frame_gl
{
    FRAME_COMPONENT_HIERARCHIC(GUIRect) {
    public:
        enum Layout { Horizontal, Vertical };

    public:
        GUIRect(const vec2& min_size=vec2(1.0f), const vec2& margin=vec2(1.0f), Layout layout=Horizontal)
        : _min_size(min_size), _margin(margin), _layout(layout) {}
        ~GUIRect() {}

    public:
        vec2 size() const { return _bottom_right - _top_left; }
        float width() const { return _bottom_right.x - _top_left.x; }
        float height() const { return _bottom_right.y - _top_left.y; }
        const vec2& min_size() const { return _min_size; }
        const vec2& max_size() const { return _max_size; }
        const vec2& margin() const { return _margin; }
        const vec2& top_left() const { return _top_left; }
        const vec2& bottom_right() const { return _bottom_right; }
        vec2 top_right() const { return vec2(_bottom_right.x, _top_left.y); }
        vec2 bottom_left() const { return vec2(_top_left.x, _bottom_right.y); }
        Layout layout() const { return _layout; }

    public:
        GUIRect* set_max_size(const vec2& max_size) { _max_size = max_size; return this; }
        GUIRect* set_min_size(const vec2& min_size) { _min_size = min_size; return this; }

        void fit(const vec2& top_left_min, const vec2& bottom_right_max) {

            // Get the max possible size
            vec2 max_size_allowed = bottom_right_max - top_left_min;
            vec2 size_plus_margin(
                _max_size.x > 0.0f ? min(max_size_allowed.x, _max_size.x) : max_size_allowed.x,
                _max_size.y > 0.0f ? min(max_size_allowed.y, _max_size.y) : max_size_allowed.y);

            // Set position
            _top_left = top_left_min + _margin;
            _bottom_right = top_left_min + size_plus_margin - _margin;

            // Re-fit children
            if (children().size() > 0) {

                if (_layout == Horizontal) {
                    float left_position = _top_left.x;
                    for (int i = 0; i < children().size(); ++i) {

                        // Compute max width of the next child
                        float remaining_width = _bottom_right.x - left_position;
                        float child_width = remaining_width / float(children().size() - i);
                        GUIRect* child = children()[i];
                        child->fit(
                            vec2(left_position, _top_left.y),
                            vec2(left_position + child_width, _bottom_right.y));

                        // Get the actual resulting child width and update state
                        child_width = child->width() + 2.0f * child->margin().x;
                        left_position += child_width;
                    }

                } else if (_layout == Vertical) {
                    float total_height = _bottom_right.y - _top_left.y;
                    float child_height = total_height / float(children().size());
                    for (int i = 0; i < children().size(); ++i) {
                        GUIRect* child = children()[i];
                        child->fit(
                            vec2(_top_left.x, _top_left.y + float(i) * child_height),
                            vec2(_bottom_right.x, _top_left.y + float(i+1) * child_height));
                    }
                }
            }
        }

        bool inside(const vec2& screen_position) {
            return (
                screen_position.x > top_left().x &&
                screen_position.y > top_left().y &&
                screen_position.x < bottom_right().x &&
                screen_position.y < bottom_right().y);
        }

    private:
        vec2 _min_size;
        vec2 _max_size;
        vec2 _margin;
        vec2 _top_left;
        vec2 _bottom_right;
        Layout _layout;
    };
}