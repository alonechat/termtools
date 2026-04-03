#pragma once
#include "primitive.h"
#include <vector>
#include <string>

namespace tt {
namespace pixel {

class ImagePrimitive : public Primitive {
public:
    ImagePrimitive(uint16_t x, uint16_t y, const std::vector<std::string>& ascii)
        : x_(x), y_(y), ascii_(ascii) {}

    void draw(Canvas& canvas) override {
        for (size_t py = 0; py < ascii_.size(); ++py) {
            const auto& line = ascii_[py];
            for (size_t px = 0; px < line.length(); ++px) {
                if (line[px] != ' ' || fg_.alpha > 0) {
                    canvas.setPixel(x_ + px, y_ + py, line[px], fg_, bg_);
                }
            }
        }
    }

    Rect bounds() const override {
        uint16_t max_w = 0;
        for (const auto& line : ascii_) {
            if (line.length() > max_w) max_w = static_cast<uint16_t>(line.length());
        }
        return Rect(x_, y_, max_w, static_cast<uint16_t>(ascii_.size()));
    }

    void setDefaultColor(Color fg, Color bg) {
        fg_ = fg;
        bg_ = bg;
    }

private:
    uint16_t x_, y_;
    std::vector<std::string> ascii_;
    Color fg_ = Color::white();
    Color bg_ = Color::black();
};

}
}