#include "termtools/text/text.h"
#include "../core/internal.h"
#include <sstream>
#include <algorithm>

namespace termtools {

Text::Text() {}

Text::Text(const std::string& content) {
    Segment seg;
    seg.content = content;
    segments_.push_back(seg);
}

Text::Text(const std::string& content, const Color& color) {
    Segment seg;
    seg.content = content;
    seg.style.color(color);
    segments_.push_back(seg);
}

Text::Text(const std::string& content, const StyleBuilder& style) {
    Segment seg;
    seg.content = content;
    seg.style = style;
    segments_.push_back(seg);
}

Text& Text::append(const Text& other) {
    for (const auto& seg : other.segments_) {
        segments_.push_back(seg);
    }
    return *this;
}

Text& Text::append(const std::string& str) {
    Segment seg;
    seg.content = str;
    segments_.push_back(seg);
    return *this;
}

Text& Text::append(const std::string& str, const Color& color) {
    Segment seg;
    seg.content = str;
    seg.style.color(color);
    segments_.push_back(seg);
    return *this;
}

Text& Text::append(const std::string& str, const StyleBuilder& style) {
    Segment seg;
    seg.content = str;
    seg.style = style;
    segments_.push_back(seg);
    return *this;
}

Text& Text::prepend(const Text& other) {
    std::vector<Segment> newSegments = other.segments_;
    newSegments.insert(newSegments.end(), segments_.begin(), segments_.end());
    segments_ = newSegments;
    return *this;
}

Text& Text::prepend(const std::string& str) {
    Segment seg;
    seg.content = str;
    segments_.insert(segments_.begin(), seg);
    return *this;
}

Text& Text::styled(const StyleBuilder& style) {
    for (auto& seg : segments_) {
        seg.style = style;
    }
    return *this;
}

Text& Text::colored(const Color& color) {
    for (auto& seg : segments_) {
        seg.style.color(color);
    }
    return *this;
}

Text& Text::justify(Justify justify, int width) {
    return align(justify, width);
}

Text& Text::align(Justify justify, int width) {
    if (width <= 0) {
        width = internal::getTerminalWidth();
    }
    
    int currentWidth = calculateWidth();
    if (currentWidth >= width) {
        return *this;
    }
    
    int padding = width - currentWidth;
    
    if (!segments_.empty()) {
        switch (justify) {
            case Justify::Left:
                segments_.back().content += std::string(padding, ' ');
                break;
            case Justify::Right:
                segments_.front().content = std::string(padding, ' ') + segments_.front().content;
                break;
            case Justify::Center:
                int leftPad = padding / 2;
                int rightPad = padding - leftPad;
                segments_.front().content = std::string(leftPad, ' ') + segments_.front().content;
                segments_.back().content += std::string(rightPad, ' ');
                break;
        }
    }
    
    return *this;
}

Text& Text::padLeft(int width, char fill) {
    int currentWidth = calculateWidth();
    if (currentWidth < width && !segments_.empty()) {
        int padding = width - currentWidth;
        segments_.front().content = std::string(padding, fill) + segments_.front().content;
    }
    return *this;
}

Text& Text::padRight(int width, char fill) {
    int currentWidth = calculateWidth();
    if (currentWidth < width && !segments_.empty()) {
        int padding = width - currentWidth;
        segments_.back().content += std::string(padding, fill);
    }
    return *this;
}

Text& Text::truncate(int max_width, Overflow overflow) {
    if (overflow == Overflow::Ignore) {
        return *this;
    }
    
    int currentWidth = calculateWidth();
    if (currentWidth <= max_width) {
        return *this;
    }
    
    switch (overflow) {
        case Overflow::Crop:
            {
                std::string plainText = plain();
                std::string cropped = internal::truncateText(plainText, max_width, "");
                segments_.clear();
                Segment seg;
                seg.content = cropped;
                segments_.push_back(seg);
            }
            break;
        case Overflow::Ellipsis:
            {
                std::string plainText = plain();
                std::string truncated = internal::truncateText(plainText, max_width, "...");
                segments_.clear();
                Segment seg;
                seg.content = truncated;
                segments_.push_back(seg);
            }
            break;
        case Overflow::Fold:
            {
                std::string plainText = plain();
                std::string wrapped = internal::wrapText(plainText, max_width);
                segments_.clear();
                Segment seg;
                seg.content = wrapped;
                segments_.push_back(seg);
            }
            break;
        default:
            break;
    }
    
    return *this;
}

std::string Text::plain() const {
    std::string result;
    for (const auto& seg : segments_) {
        result += seg.content;
    }
    return result;
}

std::string Text::render() const {
    std::ostringstream oss;
    for (const auto& seg : segments_) {
        oss << seg.style.toAnsi() << seg.content << seg.style.toAnsiReset();
    }
    return oss.str();
}

int Text::width() const {
    return calculateWidth();
}

int Text::length() const {
    return plain().length();
}

bool Text::empty() const {
    return segments_.empty() || plain().empty();
}

int Text::calculateWidth() const {
    int width = 0;
    for (const auto& seg : segments_) {
        width += internal::measureTextWidth(seg.content);
    }
    return width;
}

Text Text::fromMarkup(const std::string& markup) {
    Text result;
    std::string currentText;
    StyleBuilder currentStyle;
    
    size_t i = 0;
    while (i < markup.size()) {
        if (markup[i] == '[' && i + 1 < markup.size()) {
            size_t endBracket = markup.find(']', i);
            if (endBracket != std::string::npos) {
                if (!currentText.empty()) {
                    result.append(currentText, currentStyle);
                    currentText.clear();
                }
                
                std::string tag = markup.substr(i + 1, endBracket - i - 1);
                
                if (tag == "bold") {
                    currentStyle.bold();
                } else if (tag == "/bold") {
                    currentStyle.bold(false);
                } else if (tag == "italic") {
                    currentStyle.italic();
                } else if (tag == "/italic") {
                    currentStyle.italic(false);
                } else if (tag == "underline") {
                    currentStyle.underline();
                } else if (tag == "/underline") {
                    currentStyle.underline(false);
                } else if (tag == "dim") {
                    currentStyle.dim();
                } else if (tag == "/dim") {
                    currentStyle.dim(false);
                } else if (tag == "red") {
                    currentStyle.color(Color(ColorName::Red));
                } else if (tag == "green") {
                    currentStyle.color(Color(ColorName::Green));
                } else if (tag == "blue") {
                    currentStyle.color(Color(ColorName::Blue));
                } else if (tag == "yellow") {
                    currentStyle.color(Color(ColorName::Yellow));
                } else if (tag == "magenta") {
                    currentStyle.color(Color(ColorName::Magenta));
                } else if (tag == "cyan") {
                    currentStyle.color(Color(ColorName::Cyan));
                } else if (tag == "white") {
                    currentStyle.color(Color(ColorName::White));
                } else if (tag[0] == '#') {
                    if (tag.length() == 7) {
                        uint8_t r = std::stoi(tag.substr(1, 2), nullptr, 16);
                        uint8_t g = std::stoi(tag.substr(3, 2), nullptr, 16);
                        uint8_t b = std::stoi(tag.substr(5, 2), nullptr, 16);
                        currentStyle.color(Color(r, g, b));
                    }
                }
                
                i = endBracket + 1;
                continue;
            }
        }
        
        currentText += markup[i];
        i++;
    }
    
    if (!currentText.empty()) {
        result.append(currentText, currentStyle);
    }
    
    return result;
}

}
