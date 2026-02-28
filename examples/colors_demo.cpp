#include "termtools.h"

using namespace termtools;

int main() {
    Console console;
    
    console.println("=== 16 Standard Colors ===");
    console.println();
    
    ColorName colors[] = {
        ColorName::Black, ColorName::Red, ColorName::Green, ColorName::Yellow,
        ColorName::Blue, ColorName::Magenta, ColorName::Cyan, ColorName::White,
        ColorName::BrightBlack, ColorName::BrightRed, ColorName::BrightGreen, ColorName::BrightYellow,
        ColorName::BrightBlue, ColorName::BrightMagenta, ColorName::BrightCyan, ColorName::BrightWhite
    };
    
    const char* names[] = {
        "Black", "Red", "Green", "Yellow",
        "Blue", "Magenta", "Cyan", "White",
        "Bright Black", "Bright Red", "Bright Green", "Bright Yellow",
        "Bright Blue", "Bright Magenta", "Bright Cyan", "Bright White"
    };
    
    for (int i = 0; i < 16; ++i) {
        Text text(names[i]);
        text.colored(Color(colors[i]));
        console.println(text);
    }
    
    console.println();
    console.println("=== 256 Color Palette ===");
    console.println();
    
    for (int i = 0; i < 256; ++i) {
        Text text("█");
        text.colored(Color(static_cast<uint8_t>(i)));
        console.print(text.render());
        
        if ((i + 1) % 32 == 0) {
            console.println();
        }
    }
    
    console.println();
    console.println("=== True Color (RGB) Gradient ===");
    console.println();
    
    for (int i = 0; i < 256; ++i) {
        Text text("█");
        text.colored(Color(static_cast<uint8_t>(i), static_cast<uint8_t>(255 - i), static_cast<uint8_t>(128)));
        console.print(text.render());
    }
    
    console.println();
    console.println();
    
    console.println("=== Background Colors ===");
    console.println();
    
    for (int i = 0; i < 8; ++i) {
        StyleBuilder style;
        style.bgColor(Color(colors[i]));
        
        if (i < 4) {
            style.color(Color(ColorName::Black));
        }
        
        Text text(" Background Color Test ");
        text.styled(style);
        console.println(text);
    }
    
    return 0;
}
