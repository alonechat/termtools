#ifndef TERMTOOLS_UI_HEADING_H
#define TERMTOOLS_UI_HEADING_H

#include "termtools/core/formatting/style.h"
#include "termtools/core/formatting/text.h"
#include <string>

namespace termtools {

enum class HeadingLevel : uint8_t {
    H1 = 1,
    H2 = 2,
    H3 = 3,
    H4 = 4,
    H5 = 5,
    H6 = 6
};

class Heading {
  public:
    Heading();
    Heading(HeadingLevel level, const std::string &text);
    Heading(HeadingLevel level, const Text &text);
    
    Heading &setLevel(HeadingLevel level);
    Heading &setText(const std::string &text);
    Heading &setText(const Text &text);
    Heading &setColor(const Color &color);
    Heading &setBold(bool bold = true);
    Heading &setPrefix(const std::string &prefix);
    Heading &setStyle(const StyleBuilder &style);
    
    HeadingLevel getLevel() const;
    std::string getText() const;
    
    std::string render() const;
    void print() const;
    
    static std::string render(HeadingLevel level, const std::string &text, 
                               const Color &color = Color(ColorName::Cyan));
    static void print(HeadingLevel level, const std::string &text,
                      const Color &color = Color(ColorName::Cyan));

  private:
    HeadingLevel level_;
    Text text_;
    Color color_;
    bool bold_;
    std::string customPrefix_;
    
    std::string getPrefix() const;
};

}

#endif
