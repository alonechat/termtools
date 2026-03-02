#include "termtools.h"
#include <iostream>

using namespace termtools;

int main() {
    Console console;
    
    console.println("=== Keyboard Demo ===");
    console.println();
    console.println("Press keys to see their codes. Press 'q' to quit.");
    console.println();
    
    auto guard = Keyboard::scopeRawMode();
    
    while (true) {
        auto eventOpt = Keyboard::readKey();
        if (!eventOpt) continue;
        
        KeyEvent event = *eventOpt;
        
        if (event.isCharacter() && (event.character == 'q' || event.character == 'Q')) {
            break;
        }
        
        std::cout << "Key: " << Keyboard::keyCodeToString(event.code);
        
        if (event.isCharacter()) {
            std::cout << " | Char: '" << event.toString() << "'";
        }
        
        if (event.modifiers.ctrl) std::cout << " | Ctrl";
        if (event.modifiers.alt) std::cout << " | Alt";
        if (event.modifiers.shift) std::cout << " | Shift";
        
        std::cout << std::endl;
    }
    
    console.println();
    console.println("Keyboard demo finished.");
    
    return 0;
}
