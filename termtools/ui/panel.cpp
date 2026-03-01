#include "termtools/ui/panel.h"
#include "../core/internal.h"
#include <sstream>

namespace termtools {

Panel::Panel(const std::string& title, const Text& content)
    : title_(title)
    , content_(content)
    , titleColor_(ColorName::Cyan)
    , padding_(1)
    , expand_(false)
    , width_(0) {}

Panel::Panel(const std::string& title, const std::string& content)
    : title_(title)
    , content_(Text(content))
    , titleColor_(ColorName::Cyan)
    , padding_(1)
    , expand_(false)
    , width_(0) {}

Panel& Panel::setBorderStyle(const StyleBuilder& style) {
    borderStyle_ = style;
    return *this;
}

Panel& Panel::setTitleColor(const Color& color) {
    titleColor_ = color;
    return *this;
}

Panel& Panel::setPadding(int padding) {
    padding_ = padding;
    return *this;
}

Panel& Panel::setExpand(bool expand) {
    expand_ = expand;
    return *this;
}

Panel& Panel::setWidth(int width) {
    width_ = width;
    return *this;
}

int Panel::calculateWidth() const {
    if (width_ > 0) {
        return width_;
    }
    
    if (expand_) {
        return internal::getTerminalWidth();
    }
    
    int contentWidth = content_.width();
    int titleWidth = internal::measureTextWidth(title_);
    
    return std::max(contentWidth, titleWidth) + padding_ * 2 + 2;
}

std::string Panel::renderBorder(int width, bool top, bool bottom) const {
    std::ostringstream oss;
    
    oss << borderStyle_.toAnsi();
    
    if (top) {
        oss << "╭";
        for (int i = 0; i < width - 2; ++i) {
            oss << "─";
        }
        oss << "╮";
    } else if (bottom) {
        oss << "╰";
        for (int i = 0; i < width - 2; ++i) {
            oss << "─";
        }
        oss << "╯";
    } else {
        oss << "│";
    }
    
    oss << borderStyle_.toAnsiReset();
    
    return oss.str();
}

std::string Panel::render() const {
    std::ostringstream oss;
    
    int width = calculateWidth();
    
    oss << renderBorder(width, true, false) << "\n";
    
    if (!title_.empty()) {
        oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset();
        
        StyleBuilder titleStyle;
        titleStyle.color(titleColor_).bold();
        
        std::string padding(padding_, ' ');
        oss << padding << titleStyle.toAnsi() << title_ << titleStyle.toAnsiReset();
        
        int titleSpace = width - 2 - padding_ - internal::measureTextWidth(title_);
        if (titleSpace > padding_) {
            oss << std::string(titleSpace, ' ');
        } else {
            oss << padding;
        }
        
        oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset() << "\n";
    }
    
    std::string plainContent = content_.plain();
    std::istringstream contentStream(plainContent);
    std::string line;
    
    while (std::getline(contentStream, line)) {
        oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset();
        
        std::string padding(padding_, ' ');
        oss << padding << line;
        
        int lineSpace = width - 2 - padding_ - internal::measureTextWidth(line);
        if (lineSpace > padding_) {
            oss << std::string(lineSpace, ' ');
        } else {
            oss << padding;
        }
        
        oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset() << "\n";
    }
    
    oss << renderBorder(width, false, true) << "\n";
    
    return oss.str();
}

void Panel::print(Console& console) const {
    console.print(render());
}

}
