#pragma once
#include "primitive.h"

namespace tt {
namespace pixel {

class CirclePrimitive : public Primitive {
public:
    CirclePrimitive(int16_t cx, int16_t cy, int16_t r)
        : cx_(cx), cy_(cy), radius_(r) {}

    void draw(Canvas& canvas) override {
        if (has_border_) {
            drawBresenhamCircle(canvas, true, true);
        } else {
            drawBresenhamCircle(canvas, true, false);
        }
    }

    Rect bounds() const override {
        return Rect(cx_ - radius_, cy_ - radius_, radius_ * 2 + 1, radius_ * 2 + 1);
    }

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
    int16_t cx_, cy_, radius_;
    char fill_ch_ = ' ';
    Color fill_fg_ = Color::white();
    Color fill_bg_ = Color::black();
    char border_ch_ = 0;
    Color border_fg_;
    Color border_bg_;
    bool has_border_ = false;

    void drawBresenhamCircle(Canvas& canvas, bool border, bool fill) {
        float aspect = canvas.aspectRatio();
        int16_t x = radius_;
        int16_t y = 0;
        int16_t d = 1 - radius_;
        int16_t ra = static_cast<int16_t>(radius_ * aspect);

        auto plotCirclePoints = [&](int16_t px, int16_t py) {
            char ch = border ? border_ch_ : fill_ch_;
            Color fg = border ? border_fg_ : fill_fg_;
            Color bg = border ? border_bg_ : fill_bg_;

            canvas.setPixel(cx_ + px, cy_ + static_cast<int16_t>(py * aspect), ch, fg, bg);
            canvas.setPixel(cx_ - px, cy_ + static_cast<int16_t>(py * aspect), ch, fg, bg);
            canvas.setPixel(cx_ + px, cy_ - static_cast<int16_t>(py * aspect), ch, fg, bg);
            canvas.setPixel(cx_ - px, cy_ - static_cast<int16_t>(py * aspect), ch, fg, bg);
            canvas.setPixel(cx_ + static_cast<int16_t>(py * aspect), cy_ + px, ch, fg, bg);
            canvas.setPixel(cx_ - static_cast<int16_t>(py * aspect), cy_ + px, ch, fg, bg);
            canvas.setPixel(cx_ + static_cast<int16_t>(py * aspect), cy_ - px, ch, fg, bg);
            canvas.setPixel(cx_ - static_cast<int16_t>(py * aspect), cy_ - px, ch, fg, bg);
        };

        plotCirclePoints(x, y);

        while (x > 0) {
            if (d < 0) {
                d += 2 * y + 3;
            } else {
                d += 2 * (y - x) + 5;
                x--;
            }
            y++;
            plotCirclePoints(x, y);
        }

        if (fill) {
            for (int16_t dy = -ra; dy <= ra; ++dy) {
                for (int16_t dx = -radius_; dx <= radius_; ++dx) {
                    if (dx * dx + (dy * dy) / (aspect * aspect) <= radius_ * radius_) {
                        canvas.setPixel(cx_ + dx, cy_ + dy, fill_ch_, fill_fg_, fill_bg_);
                    }
                }
            }
        }
    }
};

}
}