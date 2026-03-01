#ifndef TERMTOOLS_PANEL_H
#define TERMTOOLS_PANEL_H

#include "termtools/color/color.h"
#include "termtools/core/console.h"
#include "termtools/core/style.h"
#include "termtools/text/text.h"

#include <string>

namespace termtools {

class Panel {
  public:
    Panel(const std::string &title, const Text &content);
    Panel(const std::string &title, const std::string &content);

    Panel &setBorderStyle(const StyleBuilder &style);
    Panel &setTitleColor(const Color &color);
    Panel &setPadding(int padding);
    Panel &setExpand(bool expand);
    Panel &setWidth(int width);

    std::string render() const;
    void print(Console &console) const;

  private:
    std::string title_;
    Text content_;
    StyleBuilder borderStyle_;
    Color titleColor_;
    int padding_;
    bool expand_;
    int width_;

    int calculateWidth() const;
    std::string renderBorder(int width, bool top = false,
                             bool bottom = false) const;
};

} 

#endif
