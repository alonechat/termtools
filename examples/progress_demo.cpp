#include "termtools.h"
#include <thread>
#include <chrono>

using namespace termtools;

int main() {
    Console& console = Console::getStdout();
    
    console.println("=== Progress Bar Demo ===");
    console.println();
    console.flush();
    
    ProgressBar progress1(100, "Downloading files", console);
    progress1.start();
    
    for (int i = 0; i < 100; ++i) {
        progress1.update(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    progress1.finish();
    
    ProgressBar progress2(50, "Processing data", console);
    progress2.setColor(Color(ColorName::Magenta));
    progress2.start();
    
    for (int i = 0; i < 50; ++i) {
        progress2.update(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    progress2.finish();
    
    ProgressBar progress3(200, "Installing packages", console);
    progress3.setColor(Color(ColorName::Cyan));
    progress3.setCompletedPrefix("✓ ");
    progress3.start();
    
    for (int i = 0; i < 200; ++i) {
        progress3.update(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
    }
    progress3.finish();
    
    console.println("All tasks completed!");
    
    return 0;
}
