#include "termtools.h"
#include <iostream>

using namespace termtools;

int main() {
    Console console;
    
    console.println("=== Keyboard Demo ===");
    console.println();
    console.println("Press keys to see their codes. Press 'q' to quit.");
    console.println();
    
    Keyboard::setRawMode(true);
    
    while (true) {
        Keyboard::KeyEvent event = Keyboard::readKey(true);
        
        if (event.ch == 'q' || event.ch == 'Q') {
            break;
        }
        
        std::cout << "Key: " << Keyboard::keyToString(event.key);
        
        if (event.ch != '\0') {
            std::cout << " | Char: '" << event.ch << "'";
        }
        
        if (event.ctrl) std::cout << " | Ctrl";
        if (event.alt) std::cout << " | Alt";
        if (event.shift) std::cout << " | Shift";
        
        std::cout << std::endl;
    }
    
    Keyboard::setRawMode(false);
    
    console.println();
    console.println("Keyboard demo finished.");
    
    return 0;
}
