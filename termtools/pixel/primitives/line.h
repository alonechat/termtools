#pragma once
#include "primitive.h"

namespace tt {
namespace pixel {

class LinePrimitive : public Primitive {
public:
    LinePrimitive(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
        : x0_(x0), y0_(y0), x1_(x1), y1_(y1) {}

    void draw(Canvas& canvas) override {
        canvas.drawLine(x0_, y0_, x1_, y1_, ch_, fg_, bg_);
    }

    Rect bounds() const override {
        return Rect(
            x0_ < x1_ ? x0_ : x1_,
            y0_ < y1_ ? y0_ : y1_,
            (x0_ > x1_ ? x0_ - x1_ : x1_ - x0_) + 1,
            (y0_ > y1_ ? y0_ - y1_ : y1_ - y0_) + 1
        );
    }

    void setStyle(char ch, Color fg, Color bg) {
        ch_ = ch;
        fg_ = fg;
        bg_ = bg;
    }

private:
    int16_t x0_, y0_, x1_, y1_;
    char ch_ = '*';
    Color fg_ = Color::white();
    Color bg_ = Color::black();
};

}
}