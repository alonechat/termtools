#include "termtools/markdown/markdown.h"
#include "termtools/core/console.h"
#include "termtools/ui/heading.h"
#include "termtools/ui/codeblock.h"
#include "termtools/ui/list.h"
#include "termtools/ui/quote.h"
#include "termtools/ui/divider.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>

namespace termtools {

MarkdownConfig::MarkdownConfig()
    : headingColor_(ColorName::Cyan), codeColor_(ColorName::Yellow),
      linkColor_(ColorName::Blue), quoteColor_(ColorName::Green),
      listBulletColor_(ColorName::Magenta), hrColor_(ColorName::White),
      headingBold_(true), codeBold_(false), indentSize_(2), quotePrefix_("│ "),
      bulletPoint_("•") {}

MarkdownConfig &MarkdownConfig::headingColor(const Color &color) {
    headingColor_ = color;
    return *this;
}

MarkdownConfig &MarkdownConfig::codeColor(const Color &color) {
    codeColor_ = color;
    return *this;
}

MarkdownConfig &MarkdownConfig::linkColor(const Color &color) {
    linkColor_ = color;
    return *this;
}

MarkdownConfig &MarkdownConfig::quoteColor(const Color &color) {
    quoteColor_ = color;
    return *this;
}

MarkdownConfig &MarkdownConfig::listBulletColor(const Color &color) {
    listBulletColor_ = color;
    return *this;
}

MarkdownConfig &MarkdownConfig::hrColor(const Color &color) {
    hrColor_ = color;
    return *this;
}

MarkdownConfig &MarkdownConfig::headingBold(bool bold) {
    headingBold_ = bold;
    return *this;
}

MarkdownConfig &MarkdownConfig::codeBold(bool bold) {
    codeBold_ = bold;
    return *this;
}

MarkdownConfig &MarkdownConfig::indentSize(int size) {
    indentSize_ = size;
    return *this;
}

MarkdownConfig &MarkdownConfig::quotePrefix(const std::string &prefix) {
    quotePrefix_ = prefix;
    return *this;
}

MarkdownConfig &MarkdownConfig::bulletPoint(const std::string &bullet) {
    bulletPoint_ = bullet;
    return *this;
}

const Color &MarkdownConfig::getHeadingColor() const { return headingColor_; }
const Color &MarkdownConfig::getCodeColor() const { return codeColor_; }
const Color &MarkdownConfig::getLinkColor() const { return linkColor_; }
const Color &MarkdownConfig::getQuoteColor() const { return quoteColor_; }
const Color &MarkdownConfig::getListBulletColor() const {
    return listBulletColor_;
}
const Color &MarkdownConfig::getHrColor() const { return hrColor_; }

bool MarkdownConfig::isHeadingBold() const { return headingBold_; }
bool MarkdownConfig::isCodeBold() const { return codeBold_; }

int MarkdownConfig::getIndentSize() const { return indentSize_; }
const std::string &MarkdownConfig::getQuotePrefix() const {
    return quotePrefix_;
}
const std::string &MarkdownConfig::getBulletPoint() const {
    return bulletPoint_;
}

MarkdownConfig MarkdownConfig::defaultConfig() { return MarkdownConfig(); }

MarkdownConfig MarkdownConfig::darkTheme() {
    MarkdownConfig config;
    config.headingColor(Color(100, 200, 255))
        .codeColor(Color(255, 200, 100))
        .linkColor(Color(100, 150, 255))
        .quoteColor(Color(150, 255, 150))
        .listBulletColor(Color(255, 150, 255))
        .hrColor(Color(200, 200, 200));
    return config;
}

MarkdownConfig MarkdownConfig::colorfulTheme() {
    MarkdownConfig config;
    config.headingColor(Color(255, 100, 100))
        .codeColor(Color(100, 255, 100))
        .linkColor(Color(100, 100, 255))
        .quoteColor(Color(255, 255, 100))
        .listBulletColor(Color(255, 100, 255))
        .hrColor(Color(100, 255, 255));
    return config;
}

MarkdownParser::MarkdownParser() : config_(MarkdownConfig::defaultConfig()) {}

MarkdownParser::MarkdownParser(const MarkdownConfig &config)
    : config_(config) {}

std::shared_ptr<MarkdownElement>
MarkdownParser::parse(const std::string &markdown) {
    auto root = std::make_shared<MarkdownElement>();
    root->type = MarkdownElementType::Paragraph;

    std::vector<std::string> lines;
    std::istringstream iss(markdown);
    std::string line;
    while (std::getline(iss, line)) {
        lines.push_back(line);
    }

    root->children = parseLines(lines);
    return root;
}

std::vector<std::shared_ptr<MarkdownElement>>
MarkdownParser::parseLines(std::vector<std::string> &lines) {
    std::vector<std::shared_ptr<MarkdownElement>> elements;
    int i = 0;

    while (i < static_cast<int>(lines.size())) {
        if (isBlankLine(lines[i])) {
            i++;
            continue;
        }

        if (isHeading(lines[i])) {
            elements.push_back(parseHeading(lines[i]));
            i++;
        } else if (isCodeBlockStart(lines[i])) {
            elements.push_back(parseCodeBlock(lines[i], lines, i));
        } else if (isHorizontalRule(lines[i])) {
            elements.push_back(parseHorizontalRule(lines[i]));
            i++;
        } else if (isBlockquote(lines[i])) {
            elements.push_back(parseBlockquote(lines[i]));
            i++;
        } else if (isUnorderedList(lines[i])) {
            elements.push_back(parseUnorderedList(lines[i]));
            i++;
        } else if (isOrderedList(lines[i])) {
            elements.push_back(parseOrderedList(lines[i]));
            i++;
        } else {
            std::string paragraphContent;
            while (i < static_cast<int>(lines.size()) &&
                   !isBlankLine(lines[i]) && !isHeading(lines[i]) &&
                   !isCodeBlockStart(lines[i]) && !isHorizontalRule(lines[i]) &&
                   !isBlockquote(lines[i]) && !isUnorderedList(lines[i]) &&
                   !isOrderedList(lines[i])) {
                if (!paragraphContent.empty()) {
                    paragraphContent += "\n";
                }
                paragraphContent += lines[i];
                i++;
            }
            elements.push_back(parseParagraph(paragraphContent));
        }
    }

    return elements;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseHeading(const std::string &line) {
    auto element = std::make_shared<MarkdownElement>();

    int level = 0;
    size_t i = 0;
    while (i < line.size() && line[i] == '#') {
        level++;
        i++;
    }

    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }

    std::string content = line.substr(i);

    switch (level) {
    case 1:
        element->type = MarkdownElementType::Heading1;
        break;
    case 2:
        element->type = MarkdownElementType::Heading2;
        break;
    case 3:
        element->type = MarkdownElementType::Heading3;
        break;
    case 4:
        element->type = MarkdownElementType::Heading4;
        break;
    case 5:
        element->type = MarkdownElementType::Heading5;
        break;
    default:
        element->type = MarkdownElementType::Heading6;
        break;
    }

    element->level = level;
    element->content = trim(content);
    parseInlineElements(element);

    return element;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseCodeBlock(const std::string &line,
                               std::vector<std::string> &lines, int &index) {
    auto element = std::make_shared<MarkdownElement>();
    element->type = MarkdownElementType::CodeBlock;

    std::string lang = line.substr(3);
    element->language = trim(lang);

    index++;
    std::string codeContent;

    while (index < static_cast<int>(lines.size()) && lines[index] != "```") {
        if (!codeContent.empty()) {
            codeContent += "\n";
        }
        codeContent += lines[index];
        index++;
    }

    if (index < static_cast<int>(lines.size())) {
        index++;
    }

    element->content = codeContent;
    return element;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseBlockquote(const std::string &line) {
    auto element = std::make_shared<MarkdownElement>();
    element->type = MarkdownElementType::Blockquote;

    size_t i = 0;
    while (i < line.size() && line[i] == '>') {
        i++;
    }
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }

    element->content = line.substr(i);
    parseInlineElements(element);

    return element;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseUnorderedList(const std::string &line) {
    auto element = std::make_shared<MarkdownElement>();
    element->type = MarkdownElementType::ListItem;

    size_t i = 0;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }

    if (i < line.size() &&
        (line[i] == '-' || line[i] == '*' || line[i] == '+')) {
        i++;
    }

    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }

    element->content = line.substr(i);
    parseInlineElements(element);

    return element;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseOrderedList(const std::string &line) {
    auto element = std::make_shared<MarkdownElement>();
    element->type = MarkdownElementType::ListItem;
    element->ordered = true;

    size_t i = 0;
    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }

    while (i < line.size() && std::isdigit(line[i])) {
        i++;
    }

    if (i < line.size() && line[i] == '.') {
        i++;
    }

    while (i < line.size() && (line[i] == ' ' || line[i] == '\t')) {
        i++;
    }

    element->content = line.substr(i);
    parseInlineElements(element);

    return element;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseHorizontalRule(const std::string &line) {
    auto element = std::make_shared<MarkdownElement>();
    element->type = MarkdownElementType::HorizontalRule;
    return element;
}

std::shared_ptr<MarkdownElement>
MarkdownParser::parseParagraph(const std::string &line) {
    auto element = std::make_shared<MarkdownElement>();
    element->type = MarkdownElementType::Paragraph;
    element->content = line;
    parseInlineElements(element);
    return element;
}

void MarkdownParser::parseInlineElements(
    std::shared_ptr<MarkdownElement> element) {
    element->children = parseInlineText(element->content);
}

std::vector<std::shared_ptr<MarkdownElement>>
MarkdownParser::parseInlineText(const std::string &text) {
    std::vector<std::shared_ptr<MarkdownElement>> elements;
    std::string currentText;

    for (size_t i = 0; i < text.size();) {
        if (text[i] == '*' || text[i] == '_') {
            char delim = text[i];
            int count = 0;
            size_t start = i;

            while (i < text.size() && text[i] == delim) {
                count++;
                i++;
            }

            if (count == 1 || count == 2 || count == 3) {
                std::string searchDelim(count, delim);
                size_t end = text.find(searchDelim, i);

                if (end != std::string::npos) {
                    if (!currentText.empty()) {
                        auto textElem = std::make_shared<MarkdownElement>();
                        textElem->type = MarkdownElementType::Text;
                        textElem->content = currentText;
                        elements.push_back(textElem);
                        currentText.clear();
                    }

                    auto styledElem = std::make_shared<MarkdownElement>();
                    std::string content = text.substr(i, end - i);

                    if (count == 3) {
                        styledElem->type = MarkdownElementType::BoldItalic;
                    } else if (count == 2) {
                        styledElem->type = MarkdownElementType::Bold;
                    } else {
                        styledElem->type = MarkdownElementType::Italic;
                    }

                    styledElem->content = content;
                    elements.push_back(styledElem);

                    i = end + count;
                    continue;
                }
            }

            currentText += text.substr(start, count);
        } else if (text[i] == '~' && i + 1 < text.size() &&
                   text[i + 1] == '~') {
            size_t end = text.find("~~", i + 2);

            if (end != std::string::npos) {
                if (!currentText.empty()) {
                    auto textElem = std::make_shared<MarkdownElement>();
                    textElem->type = MarkdownElementType::Text;
                    textElem->content = currentText;
                    elements.push_back(textElem);
                    currentText.clear();
                }

                auto strikeElem = std::make_shared<MarkdownElement>();
                strikeElem->type = MarkdownElementType::Strikethrough;
                strikeElem->content = text.substr(i + 2, end - i - 2);
                elements.push_back(strikeElem);

                i = end + 2;
                continue;
            }

            currentText += "~~";
            i += 2;
        } else if (text[i] == '`') {
            size_t end = text.find('`', i + 1);

            if (end != std::string::npos) {
                if (!currentText.empty()) {
                    auto textElem = std::make_shared<MarkdownElement>();
                    textElem->type = MarkdownElementType::Text;
                    textElem->content = currentText;
                    elements.push_back(textElem);
                    currentText.clear();
                }

                auto codeElem = std::make_shared<MarkdownElement>();
                codeElem->type = MarkdownElementType::Code;
                codeElem->content = text.substr(i + 1, end - i - 1);
                elements.push_back(codeElem);

                i = end + 1;
                continue;
            }

            currentText += '`';
            i++;
        } else if (text[i] == '[') {
            size_t textEnd = text.find(']', i);

            if (textEnd != std::string::npos && textEnd + 1 < text.size() &&
                text[textEnd + 1] == '(') {
                size_t urlEnd = text.find(')', textEnd + 2);

                if (urlEnd != std::string::npos) {
                    if (!currentText.empty()) {
                        auto textElem = std::make_shared<MarkdownElement>();
                        textElem->type = MarkdownElementType::Text;
                        textElem->content = currentText;
                        elements.push_back(textElem);
                        currentText.clear();
                    }

                    auto linkElem = std::make_shared<MarkdownElement>();
                    linkElem->type = MarkdownElementType::Link;
                    linkElem->content = text.substr(i + 1, textEnd - i - 1);
                    linkElem->url =
                        text.substr(textEnd + 2, urlEnd - textEnd - 2);
                    elements.push_back(linkElem);

                    i = urlEnd + 1;
                    continue;
                }
            }

            currentText += text[i];
            i++;
        } else {
            currentText += text[i];
            i++;
        }
    }

    if (!currentText.empty()) {
        auto textElem = std::make_shared<MarkdownElement>();
        textElem->type = MarkdownElementType::Text;
        textElem->content = currentText;
        elements.push_back(textElem);
    }

    return elements;
}

bool MarkdownParser::isHeading(const std::string &line) {
    size_t i = 0;
    while (i < line.size() && line[i] == '#') {
        i++;
    }

    if (i >= 1 && i <= 6 && i < line.size() &&
        (line[i] == ' ' || line[i] == '\t')) {
        return true;
    }

    return false;
}

bool MarkdownParser::isCodeBlockStart(const std::string &line) {
    return line.size() >= 3 && line[0] == '`' && line[1] == '`' &&
           line[2] == '`';
}

bool MarkdownParser::isBlockquote(const std::string &line) {
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        return line[i] == '>';
    }
    return false;
}

bool MarkdownParser::isUnorderedList(const std::string &line) {
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        return (line[i] == '-' || line[i] == '*' || line[i] == '+') &&
               i + 1 < line.size() &&
               (line[i + 1] == ' ' || line[i + 1] == '\t');
    }
    return false;
}

bool MarkdownParser::isOrderedList(const std::string &line) {
    for (size_t i = 0; i < line.size(); i++) {
        if (line[i] == ' ' || line[i] == '\t')
            continue;
        if (!std::isdigit(line[i]))
            return false;
        while (i < line.size() && std::isdigit(line[i]))
            i++;
        return i < line.size() && line[i] == '.';
    }
    return false;
}

bool MarkdownParser::isHorizontalRule(const std::string &line) {
    std::string trimmed = trim(line);
    if (trimmed.empty())
        return false;

    char first = trimmed[0];
    if (first != '-' && first != '*' && first != '_')
        return false;

    for (char c : trimmed) {
        if (c != first && c != ' ' && c != '\t')
            return false;
    }

    int count = 0;
    for (char c : trimmed) {
        if (c == first)
            count++;
    }

    return count >= 3;
}

bool MarkdownParser::isBlankLine(const std::string &line) {
    for (char c : line) {
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
            return false;
    }
    return true;
}

std::string MarkdownParser::trim(const std::string &str) {
    return ltrim(rtrim(str));
}

std::string MarkdownParser::ltrim(const std::string &str) {
    size_t start = 0;
    while (start < str.size() && (str[start] == ' ' || str[start] == '\t')) {
        start++;
    }
    return str.substr(start);
}

std::string MarkdownParser::rtrim(const std::string &str) {
    size_t end = str.size();
    while (end > 0 && (str[end - 1] == ' ' || str[end - 1] == '\t')) {
        end--;
    }
    return str.substr(0, end);
}

MarkdownRenderer::MarkdownRenderer()
    : config_(MarkdownConfig::defaultConfig()), parser_(config_) {}

MarkdownRenderer::MarkdownRenderer(const MarkdownConfig &config)
    : config_(config), parser_(config) {}

std::string MarkdownRenderer::render(const std::string &markdown) {
    auto root = parser_.parse(markdown);
    listCounters_.clear();
    return render(root);
}

std::string
MarkdownRenderer::render(const std::shared_ptr<MarkdownElement> &element) {
    if (element->type == MarkdownElementType::Paragraph &&
        !element->children.empty()) {
        return render(element->children);
    }
    return renderElement(element);
}

std::string MarkdownRenderer::render(
    const std::vector<std::shared_ptr<MarkdownElement>> &elements) {
    std::ostringstream oss;
    for (const auto &elem : elements) {
        oss << renderElement(elem);
    }
    return oss.str();
}

void MarkdownRenderer::print(const std::string &markdown) {
    std::cout << render(markdown);
}

void MarkdownRenderer::print(const std::shared_ptr<MarkdownElement> &element) {
    std::cout << render(element);
}

void MarkdownRenderer::print(
    const std::vector<std::shared_ptr<MarkdownElement>> &elements) {
    std::cout << render(elements);
}

MarkdownConfig &MarkdownRenderer::config() { return config_; }
const MarkdownConfig &MarkdownRenderer::config() const { return config_; }

std::string
MarkdownRenderer::renderElement(const std::shared_ptr<MarkdownElement> &element,
                                int indentLevel) {
    switch (element->type) {
    case MarkdownElementType::Heading1:
    case MarkdownElementType::Heading2:
    case MarkdownElementType::Heading3:
    case MarkdownElementType::Heading4:
    case MarkdownElementType::Heading5:
    case MarkdownElementType::Heading6:
        return renderHeading(element);
    case MarkdownElementType::Paragraph:
        return renderParagraph(element);
    case MarkdownElementType::Code:
        return renderCode(element);
    case MarkdownElementType::CodeBlock:
        return renderCodeBlock(element);
    case MarkdownElementType::Blockquote:
        return renderBlockquote(element, indentLevel);
    case MarkdownElementType::UnorderedList:
    case MarkdownElementType::ListItem:
        if (element->ordered) {
            return renderOrderedList(element, indentLevel);
        }
        return renderUnorderedList(element, indentLevel);
    case MarkdownElementType::OrderedList:
        return renderOrderedList(element, indentLevel);
    case MarkdownElementType::HorizontalRule:
        return renderHorizontalRule();
    case MarkdownElementType::Link:
        return renderLink(element);
    default:
        return renderInlineElements(element);
    }
}

std::string MarkdownRenderer::renderHeading(
    const std::shared_ptr<MarkdownElement> &element) {
    HeadingLevel level = HeadingLevel::H1;
    switch (element->type) {
        case MarkdownElementType::Heading1: level = HeadingLevel::H1; break;
        case MarkdownElementType::Heading2: level = HeadingLevel::H2; break;
        case MarkdownElementType::Heading3: level = HeadingLevel::H3; break;
        case MarkdownElementType::Heading4: level = HeadingLevel::H4; break;
        case MarkdownElementType::Heading5: level = HeadingLevel::H5; break;
        default: level = HeadingLevel::H6; break;
    }
    
    std::string text;
    for (const auto &child : element->children) {
        if (child->type == MarkdownElementType::Text) {
            text += child->content;
        } else {
            text += renderInlineElements(child);
        }
    }
    
    Heading heading(level, text);
    heading.setColor(config_.getHeadingColor()).setBold(config_.isHeadingBold());
    return heading.render();
}

std::string MarkdownRenderer::renderParagraph(
    const std::shared_ptr<MarkdownElement> &element) {
    std::ostringstream oss;

    for (const auto &child : element->children) {
        oss << renderInlineElements(child);
    }

    oss << "\n\n";
    return oss.str();
}

std::string
MarkdownRenderer::renderCode(const std::shared_ptr<MarkdownElement> &element) {
    InlineCode code(element->content);
    code.setColor(config_.getCodeColor()).setBold(config_.isCodeBold());
    return code.render();
}

std::string MarkdownRenderer::renderCodeBlock(
    const std::shared_ptr<MarkdownElement> &element) {
    CodeBlock codeBlock(element->content, element->language);
    codeBlock.setColor(config_.getCodeColor()).setDim(true);
    return codeBlock.render();
}

std::string MarkdownRenderer::renderBlockquote(
    const std::shared_ptr<MarkdownElement> &element, int indentLevel) {
    std::string text;
    for (const auto &child : element->children) {
        text += renderInlineElements(child);
    }
    
    Quote quote(text);
    quote.setColor(config_.getQuoteColor()).setItalic(true).setPrefix(config_.getQuotePrefix());
    return quote.render();
}

std::string MarkdownRenderer::renderUnorderedList(
    const std::shared_ptr<MarkdownElement> &element, int indentLevel) {
    std::string text;
    for (const auto &child : element->children) {
        text += renderInlineElements(child);
    }
    
    ListItem item(text);
    item.setIndent(indentLevel);
    
    List list(ListType::Unordered);
    list.setColor(config_.getListBulletColor())
        .setBullet(config_.getBulletPoint())
        .setIndentSize(config_.getIndentSize())
        .addItem(item);
    return list.render();
}

std::string MarkdownRenderer::renderOrderedList(
    const std::shared_ptr<MarkdownElement> &element, int indentLevel) {
    std::string text;
    for (const auto &child : element->children) {
        text += renderInlineElements(child);
    }
    
    ListItem item(text);
    item.setIndent(indentLevel);
    
    if (listCounters_.find(indentLevel) == listCounters_.end()) {
        listCounters_[indentLevel] = 1;
    }
    
    List list(ListType::Ordered);
    list.setColor(config_.getListBulletColor())
        .setIndentSize(config_.getIndentSize())
        .setStartNumber(listCounters_[indentLevel])
        .addItem(item);
    listCounters_[indentLevel]++;
    return list.render();
}

std::string MarkdownRenderer::renderHorizontalRule() {
    Divider divider(50);
    divider.setColor(config_.getHrColor()).setDim(true).setStyle(DividerStyle::Solid);
    return divider.render();
}

std::string
MarkdownRenderer::renderLink(const std::shared_ptr<MarkdownElement> &element) {
    StyleBuilder style;
    style.color(config_.getLinkColor());
    style.underline();

    return applyStyle(element->content, style);
}

std::string MarkdownRenderer::renderInlineElements(
    const std::shared_ptr<MarkdownElement> &element) {
    StyleBuilder style;

    switch (element->type) {
    case MarkdownElementType::Bold:
        style.bold();
        return applyStyle(element->content, style);
    case MarkdownElementType::Italic:
        style.italic();
        return applyStyle(element->content, style);
    case MarkdownElementType::BoldItalic:
        style.bold().italic();
        return applyStyle(element->content, style);
    case MarkdownElementType::Strikethrough:
        style.strikethrough();
        return applyStyle(element->content, style);
    case MarkdownElementType::Code:
        return renderCode(element);
    case MarkdownElementType::Link:
        return renderLink(element);
    case MarkdownElementType::Text:
    default:
        return element->content;
    }
}

std::string MarkdownRenderer::applyStyle(const std::string &text,
                                         const StyleBuilder &style) {
    return style.toAnsi() + text + style.toAnsiReset();
}

std::string MarkdownRenderer::indent(const std::string &text, int level) {
    std::string padding(level * config_.getIndentSize(), ' ');
    return padding + text;
}

Markdown::Markdown() : renderer_() {}

Markdown::Markdown(const std::string &content)
    : content_(content), renderer_() {}

Markdown::Markdown(const std::string &content, const MarkdownConfig &config)
    : content_(content), config_(config), renderer_(config) {}

Markdown &Markdown::setContent(const std::string &content) {
    content_ = content;
    return *this;
}

Markdown &Markdown::setConfig(const MarkdownConfig &config) {
    config_ = config;
    renderer_ = MarkdownRenderer(config);
    return *this;
}

std::string Markdown::render() const { return renderer_.render(content_); }

void Markdown::print() const { std::cout << render(); }

std::shared_ptr<MarkdownElement> Markdown::parse() const {
    MarkdownParser parser(config_);
    return parser.parse(content_);
}

std::string Markdown::render(const std::string &markdown) {
    MarkdownRenderer renderer;
    return renderer.render(markdown);
}

std::string Markdown::render(const std::string &markdown,
                             const MarkdownConfig &config) {
    MarkdownRenderer renderer(config);
    return renderer.render(markdown);
}

void Markdown::print(const std::string &markdown) {
    std::cout << render(markdown);
}

void Markdown::print(const std::string &markdown,
                     const MarkdownConfig &config) {
    std::cout << render(markdown, config);
}

} // namespace termtools
