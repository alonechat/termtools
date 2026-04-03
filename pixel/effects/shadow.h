#pragma once
#include "effect.h"

namespace tt {
namespace pixel {

class ShadowEffect : public Effect {
public:
    ShadowEffect(int8_t offset_x = 1, int8_t offset_y = 1, float alpha = 0.5f)
        : offset_x_(offset_x), offset_y_(offset_y), alpha_(alpha) {}

    void apply(Pixel& pixel) const override {
        Color shadow_col = has_custom_color_ ? shadow_color_ : Color::gray(pixel.fg.r * 3 / 10);
        shadow_col.alpha = static_cast<uint8_t>(pixel.fg.alpha * alpha_);

        uint16_t shadow_x = pixel.x + offset_x_;
        uint16_t shadow_y = pixel.y + offset_y_;

        pixel.x = shadow_x;
        pixel.y = shadow_y;
        pixel.fg = shadow_col;
        pixel.bg = Color::none();
    }

    Effect* clone() const override { return new ShadowEffect(*this); }

    void setOffset(int8_t x, int8_t y) { offset_x_ = x; offset_y_ = y; }
    void setAlpha(float alpha) { alpha_ = alpha; }
    void setColor(Color c) { shadow_color_ = c; has_custom_color_ = true; }

    int8_t offsetX() const { return offset_x_; }
    int8_t offsetY() const { return offset_y_; }
    float alpha() const { return alpha_; }

private:
    int8_t offset_x_, offset_y_;
    float alpha_;
    Color shadow_color_ = Color::gray(0);
    bool has_custom_color_ = false;
};

}
}