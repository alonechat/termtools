#ifndef TERMTOOLS_UI_DIVIDER_H
#define TERMTOOLS_UI_DIVIDER_H

#include "termtools/core/formatting/style.h"
#include <string>

namespace termtools {

enum class DividerStyle : uint8_t {
    Solid,
    Dashed,
    Dotted,
    Double,
    Thick
};

class Divider {
  public:
    Divider();
    explicit Divider(int width);
    
    Divider &setWidth(int width);
    Divider &setColor(const Color &color);
    Divider &setDim(bool dim = true);
    Divider &setStyle(DividerStyle style);
    Divider &setChar(const std::string &c);
    
    int getWidth() const;
    
    std::string render() const;
    void print() const;
    
    static std::string render(int width, 
                               const Color &color,
                               DividerStyle style);
    static void print(int width,
                      const Color &color,
                      DividerStyle style);

  private:
    int width_;
    Color color_;
    bool dim_;
    DividerStyle style_;
    std::string customChar_;
    bool useCustomChar_;
    
    std::string getDividerChar() const;
};

}
#endif
