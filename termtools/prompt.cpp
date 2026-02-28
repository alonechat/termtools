#include "../termtools.h"
#include "internal.h"
#include <iostream>
#include <sstream>
#include <limits>

#if defined(_WIN32)
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace termtools {

std::string Prompt::readLine() {
    std::string line;
    std::getline(std::cin, line);
    return line;
}

bool Prompt::readYesNo(bool defaultAnswer) {
    std::string input = readLine();
    
    if (input.empty()) {
        return defaultAnswer;
    }
    
    char c = std::tolower(input[0]);
    return (c == 'y' || c == 't' || c == '1');
}

std::string Prompt::ask(const std::string& question) {
    Console console;
    Text text(question);
    text.colored(Color(ColorName::Cyan));
    console.print(text.render());
    console.print(": ");
    console.flush();
    
    return readLine();
}

std::string Prompt::ask(const std::string& question, const std::string& defaultAnswer) {
    Console console;
    
    std::ostringstream oss;
    oss << question << " [" << defaultAnswer << "]";
    
    Text text(oss.str());
    text.colored(Color(ColorName::Cyan));
    console.print(text.render());
    console.print(": ");
    console.flush();
    
    std::string answer = readLine();
    
    if (answer.empty()) {
        return defaultAnswer;
    }
    
    return answer;
}

bool Prompt::confirm(const std::string& question, bool defaultAnswer) {
    Console console;
    
    std::ostringstream oss;
    oss << question << " [" << (defaultAnswer ? "Y/n" : "y/N") << "]";
    
    Text text(oss.str());
    text.colored(Color(ColorName::Yellow));
    console.print(text.render());
    console.print(": ");
    console.flush();
    
    return readYesNo(defaultAnswer);
}

int Prompt::askInt(const std::string& question, int min, int max) {
    while (true) {
        std::string answer = ask(question);
        
        try {
            int value = std::stoi(answer);
            if (value >= min && value <= max) {
                return value;
            }
            
            Console console;
            Text error("Value must be between " + std::to_string(min) + " and " + std::to_string(max));
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        } catch (...) {
            Console console;
            Text error("Please enter a valid integer");
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        }
    }
}

int Prompt::askInt(const std::string& question, int defaultAnswer, int min, int max) {
    while (true) {
        std::string answer = ask(question, std::to_string(defaultAnswer));
        
        if (answer.empty()) {
            return defaultAnswer;
        }
        
        try {
            int value = std::stoi(answer);
            if (value >= min && value <= max) {
                return value;
            }
            
            Console console;
            Text error("Value must be between " + std::to_string(min) + " and " + std::to_string(max));
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        } catch (...) {
            Console console;
            Text error("Please enter a valid integer");
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        }
    }
}

float Prompt::askFloat(const std::string& question, float min, float max) {
    while (true) {
        std::string answer = ask(question);
        
        try {
            float value = std::stof(answer);
            if (value >= min && value <= max) {
                return value;
            }
            
            Console console;
            Text error("Value must be between " + std::to_string(min) + " and " + std::to_string(max));
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        } catch (...) {
            Console console;
            Text error("Please enter a valid number");
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        }
    }
}

float Prompt::askFloat(const std::string& question, float defaultAnswer, float min, float max) {
    while (true) {
        std::string answer = ask(question, std::to_string(defaultAnswer));
        
        if (answer.empty()) {
            return defaultAnswer;
        }
        
        try {
            float value = std::stof(answer);
            if (value >= min && value <= max) {
                return value;
            }
            
            Console console;
            Text error("Value must be between " + std::to_string(min) + " and " + std::to_string(max));
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        } catch (...) {
            Console console;
            Text error("Please enter a valid number");
            error.colored(Color(ColorName::Red));
            console.println(error.render());
        }
    }
}

std::string Prompt::askPassword(const std::string& question) {
    Console console;
    Text text(question);
    text.colored(Color(ColorName::Cyan));
    console.print(text.render());
    console.print(": ");
    console.flush();
    
    std::string password;
    
#if defined(_WIN32)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
    
    std::getline(std::cin, password);
    
    SetConsoleMode(hStdin, mode);
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    std::getline(std::cin, password);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif
    
    console.println();
    return password;
}

std::string Prompt::askChoice(const std::string& question, const std::vector<std::string>& choices) {
    Console console;
    
    Text questionText(question);
    questionText.colored(Color(ColorName::Cyan));
    console.println(questionText.render());
    
    for (size_t i = 0; i < choices.size(); ++i) {
        std::ostringstream oss;
        oss << "  " << (i + 1) << ". " << choices[i];
        console.println(oss.str());
    }
    
    int choice = askInt("Enter your choice", INT_MIN, 1, static_cast<int>(choices.size()));
    return choices[choice - 1];
}

std::string Prompt::askChoice(const std::string& question, const std::vector<std::string>& choices, const std::string& defaultChoice) {
    Console console;
    
    Text questionText(question);
    questionText.colored(Color(ColorName::Cyan));
    console.println(questionText.render());
    
    int defaultIndex = -1;
    for (size_t i = 0; i < choices.size(); ++i) {
        std::ostringstream oss;
        oss << "  " << (i + 1) << ". " << choices[i];
        
        if (choices[i] == defaultChoice) {
            oss << " (default)";
            defaultIndex = static_cast<int>(i) + 1;
        }
        
        console.println(oss.str());
    }
    
    if (defaultIndex > 0) {
        int choice = askInt("Enter your choice", defaultIndex, 1, static_cast<int>(choices.size()));
        return choices[choice - 1];
    } else {
        int choice = askInt("Enter your choice", INT_MIN, 1, static_cast<int>(choices.size()));
        return choices[choice - 1];
    }
}

std::vector<std::string> Prompt::askMultipleChoice(const std::string& question, const std::vector<std::string>& choices) {
    Console console;
    
    Text questionText(question + " (comma-separated, e.g., 1,2,3)");
    questionText.colored(Color(ColorName::Cyan));
    console.println(questionText.render());
    
    for (size_t i = 0; i < choices.size(); ++i) {
        std::ostringstream oss;
        oss << "  " << (i + 1) << ". " << choices[i];
        console.println(oss.str());
    }
    
    while (true) {
        std::string answer = ask("Enter your choices");
        
        if (answer.empty()) {
            return {};
        }
        
        std::vector<std::string> selected;
        std::istringstream iss(answer);
        std::string token;
        
        bool valid = true;
        while (std::getline(iss, token, ',')) {
            try {
                int choice = std::stoi(token);
                if (choice >= 1 && choice <= static_cast<int>(choices.size())) {
                    selected.push_back(choices[choice - 1]);
                } else {
                    valid = false;
                    break;
                }
            } catch (...) {
                valid = false;
                break;
            }
        }
        
        if (valid && !selected.empty()) {
            return selected;
        }
        
        Text error("Invalid input. Please enter comma-separated numbers between 1 and " + std::to_string(choices.size()));
        error.colored(Color(ColorName::Red));
        console.println(error.render());
    }
}

}
