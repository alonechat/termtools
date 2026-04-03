#ifndef TERMTOOLS_STYLE_H
#define TERMTOOLS_STYLE_H

#include "termtools/core/formatting/color.h"

namespace termtools {

enum class Style : uint8_t {
    Bold = 1,
    Dim = 2,
    Italic = 3,
    Underline = 4,
    Blink = 5,
    Reverse = 7,
    Hidden = 8,
    Strikethrough = 9
};

class StyleBuilder {
  public:
    StyleBuilder();
    StyleBuilder &bold(bool enable = true);
    StyleBuilder &dim(bool enable = true);
    StyleBuilder &italic(bool enable = true);
    StyleBuilder &underline(bool enable = true);
    StyleBuilder &blink(bool enable = true);
    StyleBuilder &reverse(bool enable = true);
    StyleBuilder &hidden(bool enable = true);
    StyleBuilder &strikethrough(bool enable = true);
    StyleBuilder &color(const Color &color);
    StyleBuilder &bgColor(const Color &color);

    std::string toAnsi() const;
    std::string toAnsiReset() const;

  private:
    bool bold_;
    bool dim_;
    bool italic_;
    bool underline_;
    bool blink_;
    bool reverse_;
    bool hidden_;
    bool strikethrough_;
    Color fgColor_;
    Color bgColor_;
};

} 

#endif
