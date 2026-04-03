#pragma once
#include "primitive.h"
#include <string>

namespace tt {
namespace pixel {

class TextPrimitive : public Primitive {
public:
    TextPrimitive(uint16_t x, uint16_t y, const char* text)
        : x_(x), y_(y), text_(text) {}

    void draw(Canvas& canvas) override {
        for (size_t i = 0; i < text_.length(); ++i) {
            canvas.setPixel(x_ + i, y_, text_[i], fg_, bg_, flags_);
        }
    }

    Rect bounds() const override {
        return Rect(x_, y_, static_cast<uint16_t>(text_.length()), 1);
    }

    void setStyle(Color fg, Color bg, uint8_t flags = FLAG_NONE) {
        fg_ = fg;
        bg_ = bg;
        flags_ = flags;
    }

    const std::string& text() const { return text_; }

private:
    uint16_t x_, y_;
    std::string text_;
    Color fg_ = Color::white();
    Color bg_ = Color::black();
    uint8_t flags_ = FLAG_NONE;
};

}
}