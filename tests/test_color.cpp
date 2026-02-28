#include "termtools.h"
#include <cassert>
#include <iostream>

using namespace termtools;

void test_default_color() {
    Color c;
    assert(c.isDefault());
    assert(!c.isNamed());
    assert(!c.isRGB());
    assert(!c.is256());
    std::cout << "✓ Default color test passed" << std::endl;
}

void test_named_color() {
    Color c(ColorName::Red);
    assert(!c.isDefault());
    assert(c.isNamed());
    assert(!c.isRGB());
    assert(!c.is256());
    assert(c.getName() == ColorName::Red);
    std::cout << "✓ Named color test passed" << std::endl;
}

void test_rgb_color() {
    Color c(255, 128, 64);
    assert(!c.isDefault());
    assert(!c.isNamed());
    assert(c.isRGB());
    assert(!c.is256());
    
    auto [r, g, b] = c.getRGB();
    assert(r == 255);
    assert(g == 128);
    assert(b == 64);
    std::cout << "✓ RGB color test passed" << std::endl;
}

void test_256_color() {
    Color c(static_cast<uint8_t>(42));
    assert(!c.isDefault());
    assert(!c.isNamed());
    assert(!c.isRGB());
    assert(c.is256());
    assert(c.getIndex256() == 42);
    std::cout << "✓ 256 color test passed" << std::endl;
}

void test_ansi_output() {
    Color red(ColorName::Red);
    std::string ansi = red.toAnsi(true);
    assert(!ansi.empty());
    assert(ansi.find("31") != std::string::npos);
    std::cout << "✓ ANSI output test passed" << std::endl;
}

int main() {
    std::cout << "Running Color tests..." << std::endl;
    std::cout << std::endl;
    
    test_default_color();
    test_named_color();
    test_rgb_color();
    test_256_color();
    test_ansi_output();
    
    std::cout << std::endl;
    std::cout << "All Color tests passed!" << std::endl;
    
    return 0;
}
