#ifndef TERMTOOLS_UI_CODEBLOCK_H
#define TERMTOOLS_UI_CODEBLOCK_H

#include "termtools/core/style.h"
#include <string>
#include <vector>

namespace termtools {

class CodeBlock {
  public:
    CodeBlock();
    explicit CodeBlock(const std::string &code);
    CodeBlock(const std::string &code, const std::string &language);
    
    CodeBlock &setCode(const std::string &code);
    CodeBlock &setLanguage(const std::string &language);
    CodeBlock &setColor(const Color &color);
    CodeBlock &setDim(bool dim = true);
    CodeBlock &setShowBorder(bool show = true);
    CodeBlock &setBorderStyle(const std::string &style);
    
    std::string getCode() const;
    std::string getLanguage() const;
    
    std::string render() const;
    void print() const;
    
    static std::string render(const std::string &code, const std::string &language = "",
                               const Color &color = Color(ColorName::Yellow));
    static void print(const std::string &code, const std::string &language = "",
                      const Color &color = Color(ColorName::Yellow));

  private:
    std::string code_;
    std::string language_;
    Color color_;
    bool dim_;
    bool showBorder_;
    std::string borderStyle_;
    
    std::vector<std::string> splitLines(const std::string &text) const;
};

class InlineCode {
  public:
    InlineCode();
    explicit InlineCode(const std::string &code);
    
    InlineCode &setCode(const std::string &code);
    InlineCode &setColor(const Color &color);
    InlineCode &setBold(bool bold = true);
    InlineCode &setBackground(const Color &bgColor);
    
    std::string getCode() const;
    
    std::string render() const;
    void print() const;
    
    static std::string render(const std::string &code, 
                               const Color &color = Color(ColorName::Yellow));
    static void print(const std::string &code,
                      const Color &color = Color(ColorName::Yellow));

  private:
    std::string code_;
    Color color_;
    Color bgColor_;
    bool bold_;
    bool hasBackground_;
};

}
#endif
