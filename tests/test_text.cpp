#include "termtools.h"
#include <cassert>
#include <iostream>

using namespace termtools;

void test_text_creation() {
    Text t1;
    assert(t1.empty());
    
    Text t2("Hello");
    assert(!t2.empty());
    assert(t2.plain() == "Hello");
    
    std::cout << "✓ Text creation test passed" << std::endl;
}

void test_text_append() {
    Text t("Hello");
    t.append(" ").append("World");
    
    assert(t.plain() == "Hello World");
    std::cout << "✓ Text append test passed" << std::endl;
}

void test_text_color() {
    Text t("Colored");
    t.colored(Color(ColorName::Red));
    
    std::string rendered = t.render();
    assert(!rendered.empty());
    assert(rendered.find("31") != std::string::npos);
    std::cout << "✓ Text color test passed" << std::endl;
}

void test_text_width() {
    Text t("Hello");
    assert(t.width() == 5);
    
    Text t2("你好");
    assert(t2.width() == 4);
    std::cout << "✓ Text width test passed" << std::endl;
}

int main() {
    std::cout << "Running Text tests..." << std::endl;
    std::cout << std::endl;
    
    test_text_creation();
    test_text_append();
    test_text_color();
    test_text_width();
    
    std::cout << std::endl;
    std::cout << "All Text tests passed!" << std::endl;
    
    return 0;
}
