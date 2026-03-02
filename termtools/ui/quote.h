#ifndef TERMTOOLS_UI_QUOTE_H
#define TERMTOOLS_UI_QUOTE_H

#include "termtools/core/style.h"
#include "termtools/text/text.h"
#include <string>
#include <vector>

namespace termtools {

class Quote {
  public:
    Quote();
    explicit Quote(const std::string &text);
    explicit Quote(const Text &text);
    
    Quote &setText(const std::string &text);
    Quote &setText(const Text &text);
    Quote &setColor(const Color &color);
    Quote &setItalic(bool italic = true);
    Quote &setPrefix(const std::string &prefix);
    Quote &setStyle(const StyleBuilder &style);
    
    std::string getText() const;
    
    std::string render() const;
    void print() const;
    
    static std::string render(const std::string &text, 
                               const Color &color = Color(ColorName::Green));
    static void print(const std::string &text,
                      const Color &color = Color(ColorName::Green));

  private:
    Text text_;
    Color color_;
    bool italic_;
    std::string prefix_;
    
    std::vector<std::string> splitLines(const std::string &text) const;
};

}
#endif
