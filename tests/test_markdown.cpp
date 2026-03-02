#include "termtools.h"
#include <cassert>
#include <iostream>

using namespace termtools;

void test_markdown_heading() {
    std::string md = "# Heading 1\n## Heading 2";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("Heading 1") != std::string::npos);
    assert(rendered.find("Heading 2") != std::string::npos);
    std::cout << "✓ Markdown heading test passed" << std::endl;
}

void test_markdown_bold_italic() {
    std::string md = "**bold** *italic* ***both***";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("bold") != std::string::npos);
    assert(rendered.find("italic") != std::string::npos);
    assert(rendered.find("both") != std::string::npos);
    std::cout << "✓ Markdown bold/italic test passed" << std::endl;
}

void test_markdown_code() {
    std::string md = "`inline code`";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("inline code") != std::string::npos);
    std::cout << "✓ Markdown inline code test passed" << std::endl;
}

void test_markdown_code_block() {
    std::string md = "```cpp\nint x = 5;\n```";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("int x = 5;") != std::string::npos);
    std::cout << "✓ Markdown code block test passed" << std::endl;
}

void test_markdown_list() {
    std::string md = "- Item 1\n- Item 2\n- Item 3";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("Item 1") != std::string::npos);
    assert(rendered.find("Item 2") != std::string::npos);
    assert(rendered.find("Item 3") != std::string::npos);
    std::cout << "✓ Markdown unordered list test passed" << std::endl;
}

void test_markdown_ordered_list() {
    std::string md = "1. First\n2. Second\n3. Third";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("First") != std::string::npos);
    assert(rendered.find("Second") != std::string::npos);
    assert(rendered.find("Third") != std::string::npos);
    std::cout << "✓ Markdown ordered list test passed" << std::endl;
}

void test_markdown_blockquote() {
    std::string md = "> This is a quote";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("This is a quote") != std::string::npos);
    std::cout << "✓ Markdown blockquote test passed" << std::endl;
}

void test_markdown_link() {
    std::string md = "[Example](https://example.com)";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("Example") != std::string::npos);
    std::cout << "✓ Markdown link test passed" << std::endl;
}

void test_markdown_strikethrough() {
    std::string md = "~~strikethrough~~";
    std::string rendered = Markdown::render(md);
    
    assert(!rendered.empty());
    assert(rendered.find("strikethrough") != std::string::npos);
    std::cout << "✓ Markdown strikethrough test passed" << std::endl;
}

void test_markdown_config() {
    MarkdownConfig config;
    config.headingColor(Color(255, 0, 0))
          .codeColor(Color(0, 255, 0))
          .linkColor(Color(0, 0, 255));
    
    std::string md = "# Red Heading\n`green code`";
    std::string rendered = Markdown::render(md, config);
    
    assert(!rendered.empty());
    std::cout << "✓ Markdown config test passed" << std::endl;
}

void test_markdown_empty() {
    std::string rendered = Markdown::render("");
    assert(rendered.empty() || rendered.find_first_not_of("\n\r ") == std::string::npos);
    std::cout << "✓ Markdown empty test passed" << std::endl;
}

void test_markdown_complex() {
    std::string md = R"(# Title

This is a paragraph with **bold** and *italic*.

## Subtitle

- List item 1
- List item 2

> A quote

`code`

---

Done.
)";
    
    std::string rendered = Markdown::render(md);
    assert(!rendered.empty());
    std::cout << "✓ Markdown complex test passed" << std::endl;
}

int main() {
    std::cout << "Running Markdown tests..." << std::endl;
    std::cout << std::endl;
    
    test_markdown_heading();
    test_markdown_bold_italic();
    test_markdown_code();
    test_markdown_code_block();
    test_markdown_list();
    test_markdown_ordered_list();
    test_markdown_blockquote();
    test_markdown_link();
    test_markdown_strikethrough();
    test_markdown_config();
    test_markdown_empty();
    test_markdown_complex();
    
    std::cout << std::endl;
    std::cout << "All Markdown tests passed!" << std::endl;
    
    return 0;
}
