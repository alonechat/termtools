#include "termtools.h"

using namespace termtools;

int main() {
    Console console;
    
    console.println("=== Prompt Demo ===");
    console.println();
    
    std::string name = Prompt::ask("What is your name?");
    console.println("Hello, " + name + "!");
    
    console.println();
    
    bool confirm = Prompt::confirm("Do you want to continue?", true);
    if (confirm) {
        console.println("You chose to continue.");
    } else {
        console.println("You chose to stop.");
    }
    
    console.println();
    
    int age = Prompt::askInt("How old are you?", INT_MIN, 0, 150);
    console.println("You are " + std::to_string(age) + " years old.");
    
    console.println();
    
    std::vector<std::string> choices = {"Option A", "Option B", "Option C"};
    std::string choice = Prompt::askChoice("Select an option", choices);
    console.println("You selected: " + choice);
    
    return 0;
}
