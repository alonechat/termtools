#include "termtools/ui/codeblock.h"
#include <iostream>
#include <sstream>

namespace termtools {

CodeBlock::CodeBlock()
    : color_(ColorName::Yellow)
    , dim_(true)
    , showBorder_(true)
    , borderStyle_("│ ") {
}

CodeBlock::CodeBlock(const std::string &code)
    : code_(code)
    , color_(ColorName::Yellow)
    , dim_(true)
    , showBorder_(true)
    , borderStyle_("│ ") {
}

CodeBlock::CodeBlock(const std::string &code, const std::string &language)
    : code_(code)
    , language_(language)
    , color_(ColorName::Yellow)
    , dim_(true)
    , showBorder_(true)
    , borderStyle_("│ ") {
}

CodeBlock &CodeBlock::setCode(const std::string &code) {
    code_ = code;
    return *this;
}

CodeBlock &CodeBlock::setLanguage(const std::string &language) {
    language_ = language;
    return *this;
}

CodeBlock &CodeBlock::setColor(const Color &color) {
    color_ = color;
    return *this;
}

CodeBlock &CodeBlock::setDim(bool dim) {
    dim_ = dim;
    return *this;
}

CodeBlock &CodeBlock::setShowBorder(bool show) {
    showBorder_ = show;
    return *this;
}

CodeBlock &CodeBlock::setBorderStyle(const std::string &style) {
    borderStyle_ = style;
    return *this;
}

std::string CodeBlock::getCode() const {
    return code_;
}

std::string CodeBlock::getLanguage() const {
    return language_;
}

std::vector<std::string> CodeBlock::splitLines(const std::string &text) const {
    std::vector<std::string> lines;
    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::string CodeBlock::render() const {
    StyleBuilder style;
    style.color(color_);
    if (dim_) {
        style.dim();
    }
    
    std::ostringstream oss;
    
    if (showBorder_) {
        oss << style.toAnsi() << "┌────────────────────────────────────────\n" << style.toAnsiReset();
        
        if (!language_.empty()) {
            oss << style.toAnsi() << borderStyle_ << "[" << language_ << "]\n" << style.toAnsiReset();
        }
    }
    
    std::vector<std::string> lines = splitLines(code_);
    for (const auto &line : lines) {
        if (showBorder_) {
            oss << style.toAnsi() << borderStyle_ << line << "\n" << style.toAnsiReset();
        } else {
            oss << style.toAnsi() << line << "\n" << style.toAnsiReset();
        }
    }
    
    if (showBorder_) {
        oss << style.toAnsi() << "└────────────────────────────────────────\n" << style.toAnsiReset();
    }
    
    oss << "\n";
    return oss.str();
}

void CodeBlock::print() const {
    std::cout << render() << std::flush;
}

std::string CodeBlock::render(const std::string &code, const std::string &language, const Color &color) {
    CodeBlock cb(code, language);
    cb.setColor(color);
    return cb.render();
}

void CodeBlock::print(const std::string &code, const std::string &language, const Color &color) {
    std::cout << render(code, language, color) << std::flush;
}

InlineCode::InlineCode()
    : color_(ColorName::Yellow)
    , bgColor_()
    , bold_(false)
    , hasBackground_(false) {
}

InlineCode::InlineCode(const std::string &code)
    : code_(code)
    , color_(ColorName::Yellow)
    , bgColor_()
    , bold_(false)
    , hasBackground_(false) {
}

InlineCode &InlineCode::setCode(const std::string &code) {
    code_ = code;
    return *this;
}

InlineCode &InlineCode::setColor(const Color &color) {
    color_ = color;
    return *this;
}

InlineCode &InlineCode::setBold(bool bold) {
    bold_ = bold;
    return *this;
}

InlineCode &InlineCode::setBackground(const Color &bgColor) {
    bgColor_ = bgColor;
    hasBackground_ = true;
    return *this;
}

std::string InlineCode::getCode() const {
    return code_;
}

std::string InlineCode::render() const {
    StyleBuilder style;
    style.color(color_);
    if (bold_) {
        style.bold();
    }
    if (hasBackground_) {
        style.bgColor(bgColor_);
    }
    return style.toAnsi() + code_ + style.toAnsiReset();
}

void InlineCode::print() const {
    std::cout << render() << std::flush;
}

std::string InlineCode::render(const std::string &code, const Color &color) {
    InlineCode ic(code);
    ic.setColor(color);
    return ic.render();
}

void InlineCode::print(const std::string &code, const Color &color) {
    std::cout << render(code, color) << std::flush;
}

}
