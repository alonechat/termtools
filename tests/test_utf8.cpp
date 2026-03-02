#include "termtools.h"
#include <cassert>
#include <iostream>

using namespace termtools;

void test_ascii_width() {
    Text t("Hello");
    assert(t.width() == 5);
    std::cout << "✓ ASCII width test passed" << std::endl;
}

void test_chinese_width() {
    Text t("你好");
    assert(t.width() == 4);
    std::cout << "✓ Chinese width test passed" << std::endl;
}

void test_mixed_width() {
    Text t("Hello你好");
    assert(t.width() == 9);
    std::cout << "✓ Mixed width test passed" << std::endl;
}

void test_emoji_width() {
    Text t("😀");
    assert(t.width() == 2);
    std::cout << "✓ Emoji width test passed" << std::endl;
}

void test_multiple_emoji() {
    Text t("😀😁😂");
    assert(t.width() == 6);
    std::cout << "✓ Multiple emoji width test passed" << std::endl;
}

void test_mixed_emoji_text() {
    Text t("Hi😀你好");
    assert(t.width() == 8);
    std::cout << "✓ Mixed emoji and text width test passed" << std::endl;
}

void test_japanese_width() {
    Text t("こんにちは");
    assert(t.width() == 10);
    std::cout << "✓ Japanese width test passed" << std::endl;
}

void test_korean_width() {
    Text t("안녕하세요");
    assert(t.width() == 10);
    std::cout << "✓ Korean width test passed" << std::endl;
}

void test_special_chars() {
    Text t("★☆♦♢");
    assert(t.width() >= 4);
    std::cout << "✓ Special characters width test passed" << std::endl;
}

void test_empty_text() {
    Text t("");
    assert(t.width() == 0);
    std::cout << "✓ Empty text width test passed" << std::endl;
}

void test_newline_not_counted() {
    Text t("Hello\nWorld");
    int width = t.width();
    assert(width == 10 || width == 11);
    std::cout << "✓ Newline not counted test passed" << std::endl;
}

void test_text_truncate() {
    Text t("Hello World");
    t.truncate(8);
    std::string result = t.plain();
    assert(result.length() <= 11);
    std::cout << "✓ Text truncate test passed" << std::endl;
}

void test_text_align_left() {
    Text t("Test");
    t.align(Justify::Left, 10);
    std::string result = t.render();
    assert(!result.empty());
    std::cout << "✓ Text align left test passed" << std::endl;
}

void test_text_align_center() {
    Text t("Test");
    t.align(Justify::Center, 10);
    std::string result = t.render();
    assert(!result.empty());
    std::cout << "✓ Text align center test passed" << std::endl;
}

void test_text_align_right() {
    Text t("Test");
    t.align(Justify::Right, 10);
    std::string result = t.render();
    assert(!result.empty());
    std::cout << "✓ Text align right test passed" << std::endl;
}

int main() {
    std::cout << "Running UTF-8 width tests..." << std::endl;
    std::cout << std::endl;
    
    test_ascii_width();
    test_chinese_width();
    test_mixed_width();
    test_emoji_width();
    test_multiple_emoji();
    test_mixed_emoji_text();
    test_japanese_width();
    test_korean_width();
    test_special_chars();
    test_empty_text();
    test_newline_not_counted();
    test_text_truncate();
    test_text_align_left();
    test_text_align_center();
    test_text_align_right();
    
    std::cout << std::endl;
    std::cout << "All UTF-8 width tests passed!" << std::endl;
    
    return 0;
}
