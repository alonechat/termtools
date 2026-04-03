#include "termtools/ui/heading/heading.h"
#include <iostream>

namespace termtools {

Heading::Heading()
    : level_(HeadingLevel::H1), text_(), color_(ColorName::Cyan), bold_(true) {}

Heading::Heading(HeadingLevel level, const std::string &text)
    : level_(level), text_(text), color_(ColorName::Cyan), bold_(true) {}

Heading::Heading(HeadingLevel level, const Text &text)
    : level_(level), text_(text), color_(ColorName::Cyan), bold_(true) {}

Heading &Heading::setLevel(HeadingLevel level) {
    level_ = level;
    return *this;
}

Heading &Heading::setText(const std::string &text) {
    text_ = Text(text);
    return *this;
}

Heading &Heading::setText(const Text &text) {
    text_ = text;
    return *this;
}

Heading &Heading::setColor(const Color &color) {
    color_ = color;
    return *this;
}

Heading &Heading::setBold(bool bold) {
    bold_ = bold;
    return *this;
}

Heading &Heading::setPrefix(const std::string &prefix) {
    customPrefix_ = prefix;
    return *this;
}

Heading &Heading::setStyle(const StyleBuilder &style) {
    text_.styled(style);
    return *this;
}

HeadingLevel Heading::getLevel() const { return level_; }

std::string Heading::getText() const { return text_.plain(); }

std::string Heading::getPrefix() const {
    if (!customPrefix_.empty()) {
        return customPrefix_;
    }

    switch (level_) {
    case HeadingLevel::H1:
        return "══ ";
    case HeadingLevel::H2:
        return "── ";
    case HeadingLevel::H3:
        return "▶ ";
    case HeadingLevel::H4:
        return "▸ ";
    case HeadingLevel::H5:
        return "• ";
    case HeadingLevel::H6:
        return "◦ ";
    default:
        return "◦ ";
    }
}

std::string Heading::render() const {
    StyleBuilder style;
    style.color(color_);
    if (bold_) {
        style.bold();
    }

    std::string prefix = getPrefix();
    std::string content = text_.render();

    return style.toAnsi() + prefix + content + style.toAnsiReset() + "\n\n";
}

void Heading::print() const { std::cout << render() << std::flush; }

std::string Heading::render(HeadingLevel level, const std::string &text,
                            const Color &color) {
    Heading h(level, text);
    h.setColor(color);
    return h.render();
}

void Heading::print(HeadingLevel level, const std::string &text,
                    const Color &color) {
    std::cout << render(level, text, color) << std::flush;
}

} // namespace termtools
