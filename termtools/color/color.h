#ifndef TERMTOOLS_COLOR_H
#define TERMTOOLS_COLOR_H

#include <cstdint>
#include <string>
#include <tuple>

namespace termtools {

enum class ColorName : uint8_t {
    Black = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
    BrightBlack = 8,
    BrightRed = 9,
    BrightGreen = 10,
    BrightYellow = 11,
    BrightBlue = 12,
    BrightMagenta = 13,
    BrightCyan = 14,
    BrightWhite = 15
};

class Color {
  public:
    Color();
    explicit Color(ColorName name);
    Color(uint8_t r, uint8_t g, uint8_t b);
    explicit Color(uint8_t index256);

    bool isDefault() const;
    bool isNamed() const;
    bool isRGB() const;
    bool is256() const;

    ColorName getName() const;
    std::tuple<uint8_t, uint8_t, uint8_t> getRGB() const;
    uint8_t getIndex256() const;

    std::string toAnsi(bool foreground = true) const;

  private:
    enum class Type : uint8_t { Default, Named, RGB, Index256 };
    Type type_;
    ColorName name_;
    uint8_t r_, g_, b_;
    uint8_t index256_;
};

} 

#endif
