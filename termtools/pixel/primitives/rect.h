#pragma once
#include "primitive.h"

namespace tt {
namespace pixel {

class RectPrimitive : public Primitive {
public:
    RectPrimitive(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
        : x_(x), y_(y), width_(w), height_(h) {}

    void draw(Canvas& canvas) override {
        if (has_border_) {
            for (uint16_t py = y_; py < y_ + height_; ++py) {
                canvas.setPixel(x_, py, border_ch_, border_fg_, border_bg_);
                canvas.setPixel(x_ + width_ - 1, py, border_ch_, border_fg_, border_bg_);
            }
            for (uint16_t px = x_; px < x_ + width_; ++px) {
                canvas.setPixel(px, y_, border_ch_, border_fg_, border_bg_);
                canvas.setPixel(px, y_ + height_ - 1, border_ch_, border_fg_, border_bg_);
            }
        }
        for (uint16_t py = y_ + (has_border_ ? 1 : 0); py < y_ + height_ - (has_border_ ? 1 : 0); ++py) {
            for (uint16_t px = x_ + (has_border_ ? 1 : 0); px < x_ + width_ - (has_border_ ? 1 : 0); ++px) {
                canvas.setPixel(px, py, fill_ch_, fill_fg_, fill_bg_);
            }
        }
    }

    Rect bounds() const override { return Rect(x_, y_, width_, height_); }

    void setFill(char ch, Color fg, Color bg) {
        fill_ch_ = ch;
        fill_fg_ = fg;
        fill_bg_ = bg;
    }

    void setBorder(char ch, Color fg, Color bg) {
        border_ch_ = ch;
        border_fg_ = fg;
        border_bg_ = bg;
        has_border_ = true;
    }

private:
    uint16_t x_, y_, width_, height_;
    char fill_ch_ = ' ';
    Color fill_fg_ = Color::white();
    Color fill_bg_ = Color::black();
    char border_ch_ = 0;
    Color border_fg_;
    Color border_bg_;
    bool has_border_ = false;
};

}
}