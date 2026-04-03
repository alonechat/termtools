#include "termtools/core/formatting/color.h"
#include "termtools/core/internal/internal.h"
#include <sstream>

namespace termtools {

Color::Color() : type_(Type::Default), name_(ColorName::Black), r_(0), g_(0), b_(0), index256_(0) {}

Color::Color(ColorName name) : type_(Type::Named), name_(name), r_(0), g_(0), b_(0), index256_(0) {}

Color::Color(uint8_t r, uint8_t g, uint8_t b) : type_(Type::RGB), name_(ColorName::Black), r_(r), g_(g), b_(b), index256_(0) {}

Color::Color(uint8_t index256) : type_(Type::Index256), name_(ColorName::Black), r_(0), g_(0), b_(0), index256_(index256) {}

bool Color::isDefault() const {
    return type_ == Type::Default;
}

bool Color::isNamed() const {
    return type_ == Type::Named;
}

bool Color::isRGB() const {
    return type_ == Type::RGB;
}

bool Color::is256() const {
    return type_ == Type::Index256;
}

ColorName Color::getName() const {
    return name_;
}

std::tuple<uint8_t, uint8_t, uint8_t> Color::getRGB() const {
    return std::make_tuple(r_, g_, b_);
}

uint8_t Color::getIndex256() const {
    return index256_;
}

std::string Color::toAnsi(bool foreground) const {
    if (type_ == Type::Default) {
        return foreground ? "\033[39m" : "\033[49m";
    }
    
    std::ostringstream oss;
    
    if (type_ == Type::Named) {
        int code = static_cast<int>(name_);
        if (foreground) {
            if (code < 8) {
                oss << "\033[" << (30 + code) << "m";
            } else {
                oss << "\033[" << (90 + code - 8) << "m";
            }
        } else {
            if (code < 8) {
                oss << "\033[" << (40 + code) << "m";
            } else {
                oss << "\033[" << (100 + code - 8) << "m";
            }
        }
    } else if (type_ == Type::RGB) {
        if (foreground) {
            oss << "\033[38;2;" << static_cast<int>(r_) << ";" << static_cast<int>(g_) << ";" << static_cast<int>(b_) << "m";
        } else {
            oss << "\033[48;2;" << static_cast<int>(r_) << ";" << static_cast<int>(g_) << ";" << static_cast<int>(b_) << "m";
        }
    } else if (type_ == Type::Index256) {
        if (foreground) {
            oss << "\033[38;5;" << static_cast<int>(index256_) << "m";
        } else {
            oss << "\033[48;5;" << static_cast<int>(index256_) << "m";
        }
    }
    
    return oss.str();
}

namespace colors {

inline Color black() { return Color(ColorName::Black); }
inline Color red() { return Color(ColorName::Red); }
inline Color green() { return Color(ColorName::Green); }
inline Color yellow() { return Color(ColorName::Yellow); }
inline Color blue() { return Color(ColorName::Blue); }
inline Color magenta() { return Color(ColorName::Magenta); }
inline Color cyan() { return Color(ColorName::Cyan); }
inline Color white() { return Color(ColorName::White); }

inline Color brightBlack() { return Color(ColorName::BrightBlack); }
inline Color brightRed() { return Color(ColorName::BrightRed); }
inline Color brightGreen() { return Color(ColorName::BrightGreen); }
inline Color brightYellow() { return Color(ColorName::BrightYellow); }
inline Color brightBlue() { return Color(ColorName::BrightBlue); }
inline Color brightMagenta() { return Color(ColorName::BrightMagenta); }
inline Color brightCyan() { return Color(ColorName::BrightCyan); }
inline Color brightWhite() { return Color(ColorName::BrightWhite); }

inline Color rgb(uint8_t r, uint8_t g, uint8_t b) { return Color(r, g, b); }
inline Color index(uint8_t idx) { return Color(idx); }

inline Color defaultColor() { return Color(); }

}

}
