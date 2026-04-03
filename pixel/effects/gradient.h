#pragma once
#include "effect.h"

namespace tt {
namespace pixel {

enum class GradientDirection {
    HORIZONTAL,
    VERTICAL,
    DIAGONAL,
};

class GradientEffect : public Effect {
public:
    GradientEffect(Color start, Color end, GradientDirection dir = GradientDirection::HORIZONTAL)
        : start_(start), end_(end), direction_(dir) {}

    void apply(Pixel& pixel) const override {
        float t = calculateT(pixel);
        pixel.fg = interpolate(t);
    }

    Effect* clone() const override { return new GradientEffect(*this); }

    void setDirection(GradientDirection dir) { direction_ = dir; }
    void setColors(Color start, Color end) { start_ = start; end_ = end; }

    GradientDirection direction() const { return direction_; }

private:
    float calculateT(const Pixel& pixel) const {
        switch (direction_) {
            case GradientDirection::HORIZONTAL:
                return static_cast<float>(pixel.x) / 80.0f;
            case GradientDirection::VERTICAL:
                return static_cast<float>(pixel.y) / 24.0f;
            case GradientDirection::DIAGONAL:
                return (static_cast<float>(pixel.x) / 80.0f + static_cast<float>(pixel.y) / 24.0f) / 2.0f;
        }
        return 0.5f;
    }

    Color interpolate(float t) const {
        t = t < 0.0f ? 0.0f : t > 1.0f ? 1.0f : t;
        return Color{
            static_cast<uint8_t>(start_.r + (end_.r - start_.r) * t),
            static_cast<uint8_t>(start_.g + (end_.g - start_.g) * t),
            static_cast<uint8_t>(start_.b + (end_.b - start_.b) * t),
            255
        };
    }

    Color start_, end_;
    GradientDirection direction_;
};

}
}