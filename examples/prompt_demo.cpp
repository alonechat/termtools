#include "termtools.h"

using namespace termtools;

int main() {
    Console console;
    
    console.println("=== Prompt Demo ===");
    console.println();
    
    auto nameResult = Prompt::ask("What is your name?");
    std::string name = nameResult.ok() ? nameResult.value() : "Anonymous";
    console.println("Hello, " + name + "!");
    
    console.println();
    
    bool confirm = Prompt::confirm("Do you want to continue?", true);
    if (confirm) {
        console.println("You chose to continue.");
    } else {
        console.println("You chose to stop.");
    }
    
    console.println();
    
    auto ageOpt = Prompt::askInt("How old are you?", std::nullopt, 0, 150);
    if (ageOpt) {
        console.println("You are " + std::to_string(*ageOpt) + " years old.");
    } else {
        console.println("Invalid age input.");
    }
    
    console.println();
    
    std::vector<std::string> choices = {"Option A", "Option B", "Option C"};
    std::string choice = Prompt::askChoiceValue("Select an option", choices);
    if (!choice.empty()) {
        console.println("You selected: " + choice);
    }
    
    return 0;
}
