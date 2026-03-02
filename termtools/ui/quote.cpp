#include "termtools/ui/quote.h"
#include <iostream>
#include <sstream>

namespace termtools {

Quote::Quote()
    : color_(ColorName::Green)
    , italic_(true)
    , prefix_("│ ") {
}

Quote::Quote(const std::string &text)
    : text_(text)
    , color_(ColorName::Green)
    , italic_(true)
    , prefix_("│ ") {
}

Quote::Quote(const Text &text)
    : text_(text)
    , color_(ColorName::Green)
    , italic_(true)
    , prefix_("│ ") {
}

Quote &Quote::setText(const std::string &text) {
    text_ = Text(text);
    return *this;
}

Quote &Quote::setText(const Text &text) {
    text_ = text;
    return *this;
}

Quote &Quote::setColor(const Color &color) {
    color_ = color;
    return *this;
}

Quote &Quote::setItalic(bool italic) {
    italic_ = italic;
    return *this;
}

Quote &Quote::setPrefix(const std::string &prefix) {
    prefix_ = prefix;
    return *this;
}

Quote &Quote::setStyle(const StyleBuilder &style) {
    text_.styled(style);
    return *this;
}

std::string Quote::getText() const {
    return text_.plain();
}

std::vector<std::string> Quote::splitLines(const std::string &text) const {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::string Quote::render() const {
    StyleBuilder style;
    style.color(color_);
    if (italic_) {
        style.italic();
    }
    
    std::ostringstream oss;
    std::vector<std::string> lines = splitLines(text_.plain());
    
    for (const auto &line : lines) {
        oss << style.toAnsi() << prefix_ << line << style.toAnsiReset() << "\n";
    }
    
    oss << "\n";
    return oss.str();
}

void Quote::print() const {
    std::cout << render() << std::flush;
}

std::string Quote::render(const std::string &text, const Color &color) {
    Quote q(text);
    q.setColor(color);
    return q.render();
}

void Quote::print(const std::string &text, const Color &color) {
    std::cout << render(text, color) << std::flush;
}

}
