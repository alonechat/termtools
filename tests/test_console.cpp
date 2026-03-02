#include "termtools.h"
#include <cassert>
#include <iostream>

using namespace termtools;

void test_console_creation() {
    Console console;
    assert(console.isBuffering());
    std::cout << "✓ Console creation test passed" << std::endl;
}

void test_console_print() {
    Console console;
    console.print("Test");
    console.flush();
    std::cout << "✓ Console print test passed" << std::endl;
}

void test_console_println() {
    Console console;
    console.println("Test line");
    console.flush();
    std::cout << "✓ Console println test passed" << std::endl;
}

void test_console_text() {
    Console console;
    Text text("Colored text");
    text.colored(Color(ColorName::Green));
    console.println(text);
    console.flush();
    std::cout << "✓ Console text test passed" << std::endl;
}

void test_console_buffering() {
    Console console;
    assert(console.isBuffering());
    
    console.setBuffering(false);
    assert(!console.isBuffering());
    
    console.setBuffering(true);
    assert(console.isBuffering());
    
    std::cout << "✓ Console buffering test passed" << std::endl;
}

void test_console_terminal_size() {
    Console console;
    int width = console.getTerminalWidth();
    int height = console.getTerminalHeight();
    
    assert(width > 0);
    assert(height > 0);
    std::cout << "✓ Console terminal size test passed" << std::endl;
}

void test_console_singleton() {
    Console& stdout1 = Console::getStdout();
    Console& stdout2 = Console::getStdout();
    assert(&stdout1 == &stdout2);
    
    Console& stderr1 = Console::getStderr();
    Console& stderr2 = Console::getStderr();
    assert(&stderr1 == &stderr2);
    
    std::cout << "✓ Console singleton test passed" << std::endl;
}

void test_console_cursor() {
    Console console;
    console.saveCursor();
    console.restoreCursor();
    console.showCursor(true);
    std::cout << "✓ Console cursor test passed" << std::endl;
}

void test_console_move_cursor() {
    Console console;
    console.moveCursorUp(0);
    console.moveCursorDown(0);
    console.moveCursorLeft(0);
    console.moveCursorRight(0);
    std::cout << "✓ Console move cursor test passed" << std::endl;
}

int main() {
    std::cout << "Running Console tests..." << std::endl;
    std::cout << std::endl;
    
    test_console_creation();
    test_console_print();
    test_console_println();
    test_console_text();
    test_console_buffering();
    test_console_terminal_size();
    test_console_singleton();
    test_console_cursor();
    test_console_move_cursor();
    
    std::cout << std::endl;
    std::cout << "All Console tests passed!" << std::endl;
    
    return 0;
}
