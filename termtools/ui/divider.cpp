#include "termtools/ui/divider.h"
#include <iostream>
#include <sstream>

namespace termtools {

Divider::Divider()
    : width_(50)
    , color_(ColorName::White)
    , dim_(true)
    , style_(DividerStyle::Solid)
    , customChar_("-")
    , useCustomChar_(false) {
}

Divider::Divider(int width)
    : width_(width)
    , color_(ColorName::White)
    , dim_(true)
    , style_(DividerStyle::Solid)
    , customChar_("-")
    , useCustomChar_(false) {
}

Divider &Divider::setWidth(int width) {
    width_ = width;
    return *this;
}

Divider &Divider::setColor(const Color &color) {
    color_ = color;
    return *this;
}

Divider &Divider::setDim(bool dim) {
    dim_ = dim;
    return *this;
}

Divider &Divider::setStyle(DividerStyle style) {
    style_ = style;
    useCustomChar_ = false;
    return *this;
}

Divider &Divider::setChar(const std::string &c) {
    customChar_ = c;
    useCustomChar_ = true;
    return *this;
}

int Divider::getWidth() const {
    return width_;
}

std::string Divider::getDividerChar() const {
    if (useCustomChar_) {
        return customChar_;
    }
    
    switch (style_) {
        case DividerStyle::Solid: return "─";
        case DividerStyle::Dashed: return "-";
        case DividerStyle::Dotted: return "·";
        case DividerStyle::Double: return "═";
        case DividerStyle::Thick: return "━";
        default: return "─";
    }
}

std::string Divider::render() const {
    StyleBuilder style;
    style.color(color_);
    if (dim_) {
        style.dim();
    }
    
    std::ostringstream oss;
    oss << style.toAnsi();
    
    std::string ch = getDividerChar();
    for (int i = 0; i < width_; ++i) {
        oss << ch;
    }
    
    oss << style.toAnsiReset() << "\n\n";
    return oss.str();
}

void Divider::print() const {
    std::cout << render() << std::flush;
}

std::string Divider::render(int width, const Color &color, DividerStyle style) {
    Divider d(width);
    d.setColor(color).setStyle(style);
    return d.render();
}

void Divider::print(int width, const Color &color, DividerStyle style) {
    std::cout << render(width, color, style) << std::flush;
}

}
