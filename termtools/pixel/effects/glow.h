#pragma once
#include "effect.h"

namespace tt {
namespace pixel {

class GlowEffect : public Effect {
public:
    GlowEffect(float radius = 1.0f, float intensity = 0.8f)
        : radius_(radius), intensity_(intensity) {}

    void apply(Pixel& pixel) const override {
        Color glow_col = has_custom_color_ ? glow_color_ : pixel.fg;

        uint8_t brighten = static_cast<uint8_t>(255 * intensity_ * radius_);
        uint8_t new_r = pixel.fg.r + brighten > 255 ? 255 : pixel.fg.r + brighten;
        uint8_t new_g = pixel.fg.g + brighten > 255 ? 255 : pixel.fg.g + brighten;
        uint8_t new_b = pixel.fg.b + brighten > 255 ? 255 : pixel.fg.b + brighten;

        pixel.fg.r = new_r;
        pixel.fg.g = new_g;
        pixel.fg.b = new_b;
        pixel.bg = glow_col;
    }

    Effect* clone() const override { return new GlowEffect(*this); }

    void setRadius(float r) { radius_ = r; }
    void setIntensity(float i) { intensity_ = i; }
    void setColor(Color c) { glow_color_ = c; has_custom_color_ = true; }

    float radius() const { return radius_; }
    float intensity() const { return intensity_; }

private:
    float radius_, intensity_;
    Color glow_color_ = Color::white();
    bool has_custom_color_ = false;
};

}
}