#ifndef TERMTOOLS_MARKDOWN_H
#define TERMTOOLS_MARKDOWN_H

#include "termtools/core/formatting/style.h"
#include "termtools/core/formatting/text.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace termtools {

enum class MarkdownElementType {
    Heading1,
    Heading2,
    Heading3,
    Heading4,
    Heading5,
    Heading6,
    Paragraph,
    Bold,
    Italic,
    BoldItalic,
    Strikethrough,
    Code,
    CodeBlock,
    Blockquote,
    UnorderedList,
    OrderedList,
    ListItem,
    HorizontalRule,
    Link,
    Image,
    Text
};

struct MarkdownElement {
    MarkdownElementType type;
    std::string content;
    std::vector<std::shared_ptr<MarkdownElement>> children;
    std::string url;
    std::string language;
    int level = 0;
    bool ordered = false;
};

class MarkdownConfig {
  public:
    MarkdownConfig();
    
    MarkdownConfig &headingColor(const Color &color);
    MarkdownConfig &codeColor(const Color &color);
    MarkdownConfig &linkColor(const Color &color);
    MarkdownConfig &quoteColor(const Color &color);
    MarkdownConfig &listBulletColor(const Color &color);
    MarkdownConfig &hrColor(const Color &color);
    
    MarkdownConfig &headingBold(bool bold);
    MarkdownConfig &codeBold(bool bold);
    
    MarkdownConfig &indentSize(int size);
    MarkdownConfig &quotePrefix(const std::string &prefix);
    MarkdownConfig &bulletPoint(const std::string &bullet);
    
    const Color &getHeadingColor() const;
    const Color &getCodeColor() const;
    const Color &getLinkColor() const;
    const Color &getQuoteColor() const;
    const Color &getListBulletColor() const;
    const Color &getHrColor() const;
    
    bool isHeadingBold() const;
    bool isCodeBold() const;
    
    int getIndentSize() const;
    const std::string &getQuotePrefix() const;
    const std::string &getBulletPoint() const;
    
    static MarkdownConfig defaultConfig();
    static MarkdownConfig darkTheme();
    static MarkdownConfig colorfulTheme();

  private:
    Color headingColor_;
    Color codeColor_;
    Color linkColor_;
    Color quoteColor_;
    Color listBulletColor_;
    Color hrColor_;
    
    bool headingBold_;
    bool codeBold_;
    
    int indentSize_;
    std::string quotePrefix_;
    std::string bulletPoint_;
};

class MarkdownParser {
  public:
    MarkdownParser();
    explicit MarkdownParser(const MarkdownConfig &config);
    
    std::shared_ptr<MarkdownElement> parse(const std::string &markdown);
    std::vector<std::shared_ptr<MarkdownElement>> parseLines(std::vector<std::string> &lines);
    
  private:
    MarkdownConfig config_;
    
    std::shared_ptr<MarkdownElement> parseLine(const std::string &line, int &index);
    std::shared_ptr<MarkdownElement> parseHeading(const std::string &line);
    std::shared_ptr<MarkdownElement> parseCodeBlock(const std::string &line, std::vector<std::string> &lines, int &index);
    std::shared_ptr<MarkdownElement> parseBlockquote(const std::string &line);
    std::shared_ptr<MarkdownElement> parseUnorderedList(const std::string &line);
    std::shared_ptr<MarkdownElement> parseOrderedList(const std::string &line);
    std::shared_ptr<MarkdownElement> parseHorizontalRule(const std::string &line);
    std::shared_ptr<MarkdownElement> parseParagraph(const std::string &line);
    
    void parseInlineElements(std::shared_ptr<MarkdownElement> element);
    std::vector<std::shared_ptr<MarkdownElement>> parseInlineText(const std::string &text);
    
    bool isHeading(const std::string &line);
    bool isCodeBlockStart(const std::string &line);
    bool isBlockquote(const std::string &line);
    bool isUnorderedList(const std::string &line);
    bool isOrderedList(const std::string &line);
    bool isHorizontalRule(const std::string &line);
    bool isBlankLine(const std::string &line);
    
    std::string trim(const std::string &str);
    std::string ltrim(const std::string &str);
    std::string rtrim(const std::string &str);
};

class MarkdownRenderer {
  public:
    MarkdownRenderer();
    explicit MarkdownRenderer(const MarkdownConfig &config);
    
    std::string render(const std::string &markdown);
    std::string render(const std::shared_ptr<MarkdownElement> &element);
    std::string render(const std::vector<std::shared_ptr<MarkdownElement>> &elements);
    
    void print(const std::string &markdown);
    void print(const std::shared_ptr<MarkdownElement> &element);
    void print(const std::vector<std::shared_ptr<MarkdownElement>> &elements);
    
    MarkdownConfig &config();
    const MarkdownConfig &config() const;

  private:
    MarkdownConfig config_;
    MarkdownParser parser_;
    std::map<int, int> listCounters_;
    
    std::string renderElement(const std::shared_ptr<MarkdownElement> &element, int indent = 0);
    std::string renderHeading(const std::shared_ptr<MarkdownElement> &element);
    std::string renderParagraph(const std::shared_ptr<MarkdownElement> &element);
    std::string renderCode(const std::shared_ptr<MarkdownElement> &element);
    std::string renderCodeBlock(const std::shared_ptr<MarkdownElement> &element);
    std::string renderBlockquote(const std::shared_ptr<MarkdownElement> &element, int indent = 0);
    std::string renderUnorderedList(const std::shared_ptr<MarkdownElement> &element, int indent = 0);
    std::string renderOrderedList(const std::shared_ptr<MarkdownElement> &element, int indent = 0);
    std::string renderHorizontalRule();
    std::string renderLink(const std::shared_ptr<MarkdownElement> &element);
    std::string renderInlineElements(const std::shared_ptr<MarkdownElement> &element);
    
    std::string applyStyle(const std::string &text, const StyleBuilder &style);
    std::string indent(const std::string &text, int level);
};

class Markdown {
  public:
    Markdown();
    explicit Markdown(const std::string &content);
    Markdown(const std::string &content, const MarkdownConfig &config);
    
    Markdown &setContent(const std::string &content);
    Markdown &setConfig(const MarkdownConfig &config);
    
    std::string render() const;
    void print() const;
    
    std::shared_ptr<MarkdownElement> parse() const;
    
    static std::string render(const std::string &markdown);
    static std::string render(const std::string &markdown, const MarkdownConfig &config);
    static void print(const std::string &markdown);
    static void print(const std::string &markdown, const MarkdownConfig &config);

  private:
    std::string content_;
    MarkdownConfig config_;
    mutable MarkdownRenderer renderer_;
};

}

#endif
