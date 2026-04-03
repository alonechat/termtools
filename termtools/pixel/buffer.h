#pragma once
#include <cstdint>
#include <algorithm>

namespace tt {
namespace pixel {

struct Color {
    uint8_t r, g, b;
    uint8_t alpha;
    
    static Color none() { return {0, 0, 0, 0}; }
    static Color black() { return {0, 0, 0, 255}; }
    static Color white() { return {255, 255, 255, 255}; }
    static Color red() { return {255, 0, 0, 255}; }
    static Color green() { return {0, 255, 0, 255}; }
    static Color blue() { return {0, 0, 255, 255}; }
    static Color cyan() { return {0, 255, 255, 255}; }
    static Color magenta() { return {255, 0, 255, 255}; }
    static Color yellow() { return {255, 255, 0, 255}; }
    static Color gray(uint8_t v) { return {v, v, v, 255}; }
    
    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b && alpha == other.alpha;
    }
    
    uint32_t to_rgb24() const { return (r << 16) | (g << 8) | b; }
    uint32_t to_truecolor() const { return to_rgb24(); }
};

enum PixelFlags {
    FLAG_NONE = 0,
    FLAG_BOLD = 1 << 0,
    FLAG_ITALIC = 1 << 1,
    FLAG_UNDERLINE = 1 << 2,
    FLAG_STRIKETHROUGH = 1 << 3,
    FLAG_INVERSE = 1 << 4,
};

struct Pixel {
    uint16_t x, y;
    char ch;
    Color fg;
    Color bg;
    uint8_t flags;
    int16_t z_index;
    
    Pixel() : x(0), y(0), ch(' '), fg(Color::white()), bg(Color::black()), flags(FLAG_NONE), z_index(0) {}
    
    bool operator<(const Pixel& other) const {
        if (z_index != other.z_index) return z_index < other.z_index;
        if (y != other.y) return y < other.y;
        return x < other.x;
    }
    
    bool isTransparent() const { return bg.alpha == 0; }
};

struct Rect {
    uint16_t x, y, width, height;
    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(uint16_t x, uint16_t y, uint16_t w, uint16_t h) : x(x), y(y), width(w), height(h) {}
    bool contains(uint16_t px, uint16_t py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }
};

}
}