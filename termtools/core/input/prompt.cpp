#include "termtools/core/input/prompt.h"
#include "termtools/core/internal/internal.h"
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <regex>
#include <sstream>

#if defined(_WIN32)
#include <windows.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace termtools {

namespace {

PromptStyle g_defaultStyle;

std::string formatDefault(const std::string &value, const PromptStyle &style) {
    std::string result = style.defaultFormat;
    size_t pos = result.find("{}");
    if (pos != std::string::npos) {
        result.replace(pos, 2, value);
    }
    return result;
}

std::string colorize(const std::string &text, const std::string &colorName) {
    Console console;
    Text textObj(text);

    if (colorName == "cyan") {
        textObj.colored(Color(ColorName::Cyan));
    } else if (colorName == "yellow") {
        textObj.colored(Color(ColorName::Yellow));
    } else if (colorName == "red") {
        textObj.colored(Color(ColorName::Red));
    } else if (colorName == "green") {
        textObj.colored(Color(ColorName::Green));
    } else if (colorName == "dim") {
        textObj.colored(Color(ColorName::BrightBlack));
    } else if (colorName == "white") {
        textObj.colored(Color(ColorName::White));
    } else if (colorName == "magenta") {
        textObj.colored(Color(ColorName::Magenta));
    } else if (colorName == "blue") {
        textObj.colored(Color(ColorName::Blue));
    }

    return textObj.render();
}

std::string trim(const std::string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool parseBool(const std::string &input, bool defaultValue) {
    std::string lower;
    lower.reserve(input.size());
    for (char c : input) {
        lower += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    if (lower == "y" || lower == "yes" || lower == "true" || lower == "1" ||
        lower == "on") {
        return true;
    }
    if (lower == "n" || lower == "no" || lower == "false" || lower == "0" ||
        lower == "off") {
        return false;
    }

    return defaultValue;
}

} // namespace

PromptStyle &Prompt::getMutableStyle() { return g_defaultStyle; }

PromptStyle &Prompt::style() { return g_defaultStyle; }

void Prompt::setStyle(const PromptStyle &style) { g_defaultStyle = style; }

std::string Prompt::readLine() {
    std::string line;
    std::getline(std::cin, line);
    return trim(line);
}

std::string Prompt::readLineMasked(char maskChar) {
    std::string password;

#if defined(_WIN32)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    DWORD newMode = mode & (~ENABLE_ECHO_INPUT);
    if (maskChar != '\0') {
        newMode &= ~ENABLE_LINE_INPUT;
    }
    SetConsoleMode(hStdin, newMode);

    if (maskChar == '\0') {
        std::getline(std::cin, password);
    } else {
        char ch;
        while (std::cin.get(ch) && ch != '\n' && ch != '\r') {
            if (ch == '\b' || ch == 127) {
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b";
                }
            } else {
                password += ch;
                std::cout << maskChar;
            }
        }
        std::cout << '\n';
    }

    SetConsoleMode(hStdin, mode);
#else
    termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~ECHO;

    if (maskChar != '\0') {
        newt.c_lflag &= ~ICANON;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    if (maskChar == '\0') {
        std::getline(std::cin, password);
    } else {
        char ch;
        while (read(STDIN_FILENO, &ch, 1) > 0 && ch != '\n' && ch != '\r') {
            if (ch == 127 || ch == '\b') {
                if (!password.empty()) {
                    password.pop_back();
                    std::cout << "\b \b" << std::flush;
                }
            } else {
                password += ch;
                std::cout << maskChar << std::flush;
            }
        }
        std::cout << '\n';
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
#endif

    return password;
}

void Prompt::printQuestion(const std::string &question,
                           const std::string &suffix) {
    Console console;
    console.print(colorize(question, g_defaultStyle.questionColor));
    if (!suffix.empty()) {
        console.print(suffix);
    }
    console.print(g_defaultStyle.promptSuffix);
    console.flush();
}

void Prompt::printError(const std::string &message) {
    Console console;
    Text errorText(message);
    errorText.colored(Color(ColorName::Red));
    console.println(errorText.render());
}

void Prompt::printHint(const std::string &hint) {
    if (!g_defaultStyle.showHints)
        return;
    Console console;
    Text hintText(hint);
    hintText.colored(Color(ColorName::Yellow));
    console.println(hintText.render());
}

PromptResult Prompt::ask(const std::string &question,
                         const std::optional<std::string> &defaultValue) {
    Console console;

    std::string displayQuestion = question;
    if (defaultValue) {
        displayQuestion +=
            " " + colorize(formatDefault(*defaultValue, g_defaultStyle),
                           g_defaultStyle.defaultValueColor);
    }

    printQuestion(displayQuestion);

    std::string answer = readLine();

    if (answer.empty() && defaultValue) {
        return PromptResult(*defaultValue);
    }

    return PromptResult(answer);
}

bool Prompt::confirm(const std::string &question,
                     std::optional<bool> defaultValue) {
    Console console;

    std::string displayQuestion = question;
    if (defaultValue) {
        std::string defaultStr = *defaultValue ? "Y/n" : "y/N";
        displayQuestion +=
            " " + colorize(formatDefault(defaultStr, g_defaultStyle),
                           g_defaultStyle.defaultValueColor);
    } else {
        displayQuestion +=
            " " + colorize("[y/n]", g_defaultStyle.defaultValueColor);
    }

    printQuestion(displayQuestion);

    std::string answer = readLine();

    if (answer.empty() && defaultValue) {
        return *defaultValue;
    }

    return parseBool(answer, false);
}

std::optional<int> Prompt::askInt(const std::string &question,
                                  std::optional<int> defaultValue, int min,
                                  int max, int maxAttempts) {
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        std::string displayQuestion = question;
        if (defaultValue) {
            displayQuestion +=
                " " + colorize(formatDefault(std::to_string(*defaultValue),
                                             g_defaultStyle),
                               g_defaultStyle.defaultValueColor);
        }

        printQuestion(displayQuestion);
        std::string answer = readLine();

        if (answer.empty() && defaultValue) {
            return defaultValue;
        }

        try {
            size_t pos = 0;
            long value = std::stol(answer, &pos);

            if (pos != answer.length()) {
                printError("Invalid integer format");
                continue;
            }

            if (value < min || value > max) {
                printError("Value must be between " + std::to_string(min) +
                           " and " + std::to_string(max));
                continue;
            }

            return static_cast<int>(value);
        } catch (const std::out_of_range &) {
            printError("Value out of range");
        } catch (const std::invalid_argument &) {
            printError("Please enter a valid integer");
        }
    }

    return std::nullopt;
}

std::optional<double> Prompt::askDouble(const std::string &question,
                                        std::optional<double> defaultValue,
                                        double min, double max,
                                        int maxAttempts) {
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        std::string displayQuestion = question;
        if (defaultValue) {
            std::ostringstream oss;
            oss << *defaultValue;
            displayQuestion +=
                " " + colorize(formatDefault(oss.str(), g_defaultStyle),
                               g_defaultStyle.defaultValueColor);
        }

        printQuestion(displayQuestion);
        std::string answer = readLine();

        if (answer.empty() && defaultValue) {
            return defaultValue;
        }

        try {
            size_t pos = 0;
            double value = std::stod(answer, &pos);

            if (pos != answer.length()) {
                printError("Invalid number format");
                continue;
            }

            if (std::isnan(value) || std::isinf(value)) {
                printError("Invalid number value");
                continue;
            }

            if (value < min || value > max) {
                std::ostringstream oss;
                oss << "Value must be between " << min << " and " << max;
                printError(oss.str());
                continue;
            }

            return value;
        } catch (const std::out_of_range &) {
            printError("Value out of range");
        } catch (const std::invalid_argument &) {
            printError("Please enter a valid number");
        }
    }

    return std::nullopt;
}

std::string Prompt::askPassword(const std::string &question, bool mask,
                                char maskChar) {
    Console console;
    printQuestion(question);

    std::string password;

    if (mask) {
        password = readLineMasked(maskChar);
    } else {
        password = readLineMasked('\0');
    }

    return password;
}

std::optional<size_t> Prompt::askChoice(const std::string &question,
                                        const std::vector<std::string> &choices,
                                        std::optional<size_t> defaultIndex) {
    if (choices.empty()) {
        return std::nullopt;
    }

    Console console;

    Text questionText(question);
    questionText.colored(Color(ColorName::Cyan));
    console.println(questionText.render());

    for (size_t i = 0; i < choices.size(); ++i) {
        std::ostringstream oss;
        oss << "  " << (i + 1) << ". " << choices[i];

        if (defaultIndex && i == *defaultIndex) {
            oss << " "
                << colorize("(default)", g_defaultStyle.defaultValueColor);
        }

        console.println(oss.str());
    }

    console.flush();

    auto result = askInt(
        "Enter your choice",
        defaultIndex ? std::optional<int>(static_cast<int>(*defaultIndex) + 1)
                     : std::nullopt,
        1, static_cast<int>(choices.size()), 3);

    if (result) {
        return static_cast<size_t>(*result - 1);
    }

    return std::nullopt;
}

std::string Prompt::askChoiceValue(const std::string &question,
                                   const std::vector<std::string> &choices,
                                   const std::string &defaultValue) {
    std::optional<size_t> defaultIdx;
    if (!defaultValue.empty()) {
        auto it = std::find(choices.begin(), choices.end(), defaultValue);
        if (it != choices.end()) {
            defaultIdx = std::distance(choices.begin(), it);
        }
    }

    auto result = askChoice(question, choices, defaultIdx);

    if (result) {
        return choices[*result];
    }

    return defaultValue;
}

std::vector<size_t>
Prompt::askMultiChoice(const std::string &question,
                       const std::vector<std::string> &choices) {
    if (choices.empty()) {
        return {};
    }

    Console console;

    Text questionText(question + " (comma-separated, e.g., 1,2,3)");
    questionText.colored(Color(ColorName::Cyan));
    console.println(questionText.render());

    for (size_t i = 0; i < choices.size(); ++i) {
        std::ostringstream oss;
        oss << "  " << (i + 1) << ". " << choices[i];
        console.println(oss.str());
    }

    for (int attempt = 0; attempt < 3; ++attempt) {
        printQuestion("Enter your choices");
        std::string answer = readLine();

        if (answer.empty()) {
            return {};
        }

        std::vector<size_t> selected;
        std::istringstream iss(answer);
        std::string token;
        bool valid = true;

        while (std::getline(iss, token, ',')) {
            token = trim(token);
            if (token.empty())
                continue;

            try {
                int choice = std::stoi(token);
                if (choice >= 1 && choice <= static_cast<int>(choices.size())) {
                    size_t idx = static_cast<size_t>(choice - 1);
                    if (std::find(selected.begin(), selected.end(), idx) ==
                        selected.end()) {
                        selected.push_back(idx);
                    }
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

        printError("Invalid input. Please enter comma-separated numbers "
                   "between 1 and " +
                   std::to_string(choices.size()));
    }

    return {};
}

std::vector<std::string>
Prompt::askMultiChoiceValues(const std::string &question,
                             const std::vector<std::string> &choices) {
    auto indices = askMultiChoice(question, choices);

    std::vector<std::string> result;
    result.reserve(indices.size());

    for (size_t idx : indices) {
        result.push_back(choices[idx]);
    }

    return result;
}

std::string Prompt::askWithValidation(const std::string &question,
                                      Validator<std::string> validator,
                                      const std::string &errorMessage,
                                      std::optional<std::string> defaultValue,
                                      int maxAttempts) {
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        auto result = ask(question, defaultValue);

        if (!result.ok()) {
            return defaultValue.value_or("");
        }

        const std::string &value = result.value();

        if (value.empty() && defaultValue) {
            return *defaultValue;
        }

        if (!validator || validator(value)) {
            return value;
        }

        printError(errorMessage.empty() ? "Invalid input" : errorMessage);
    }

    return defaultValue.value_or("");
}

PromptBuilder &PromptBuilder::question(const std::string &q) {
    question_ = q;
    return *this;
}

PromptBuilder &PromptBuilder::defaultValue(const std::string &value) {
    defaultValue_ = value;
    return *this;
}

PromptBuilder &PromptBuilder::validator(Validator<std::string> v,
                                        const std::string &errorMsg) {
    validator_ = std::move(v);
    errorMessage_ = errorMsg;
    return *this;
}

PromptBuilder &PromptBuilder::hint(const std::string &h) {
    hint_ = h;
    return *this;
}

PromptBuilder &PromptBuilder::required(bool r) {
    required_ = r;
    return *this;
}

PromptBuilder &PromptBuilder::maxAttempts(int n) {
    maxAttempts_ = n;
    return *this;
}

PromptBuilder &PromptBuilder::style(const PromptStyle &s) {
    style_ = s;
    return *this;
}

PromptResult PromptBuilder::ask() {
    PromptStyle oldStyle;
    bool hasCustomStyle = style_.has_value();
    if (hasCustomStyle) {
        oldStyle = Prompt::style();
        Prompt::setStyle(*style_);
    }

    for (int attempt = 0; attempt < maxAttempts_; ++attempt) {
        if (!hint_.empty() && attempt == 0) {
            Prompt::printHint(hint_);
        }

        auto result = Prompt::ask(question_, defaultValue_);

        if (!result.ok()) {
            if (hasCustomStyle)
                Prompt::setStyle(oldStyle);
            return result;
        }

        const std::string &value = result.value();

        if (value.empty() && defaultValue_) {
            if (hasCustomStyle)
                Prompt::setStyle(oldStyle);
            return PromptResult(*defaultValue_);
        }

        if (value.empty() && required_) {
            Prompt::printError("This field is required");
            continue;
        }

        if (validator_ && !validator_(value)) {
            Prompt::printError(errorMessage_.empty() ? "Invalid input"
                                                     : errorMessage_);
            continue;
        }

        if (hasCustomStyle)
            Prompt::setStyle(oldStyle);
        return result;
    }

    if (hasCustomStyle)
        Prompt::setStyle(oldStyle);
    return PromptResult(PromptResult::Status::Invalid);
}

std::string PromptBuilder::askOr(const std::string &fallback) {
    auto result = ask();
    return result.ok() ? result.value() : fallback;
}

ChoiceBuilder &ChoiceBuilder::question(const std::string &q) {
    question_ = q;
    return *this;
}

ChoiceBuilder &ChoiceBuilder::choice(const std::string &c) {
    choices_.push_back(c);
    return *this;
}

ChoiceBuilder &ChoiceBuilder::choices(const std::vector<std::string> &cs) {
    choices_ = cs;
    return *this;
}

ChoiceBuilder &ChoiceBuilder::defaultIndex(size_t idx) {
    defaultIndex_ = idx;
    return *this;
}

ChoiceBuilder &ChoiceBuilder::allowMulti(bool allow) {
    allowMulti_ = allow;
    return *this;
}

ChoiceBuilder &ChoiceBuilder::style(const PromptStyle &s) {
    style_ = s;
    return *this;
}

std::optional<size_t> ChoiceBuilder::ask() {
    PromptStyle oldStyle;
    bool hasCustomStyle = style_.has_value();
    if (hasCustomStyle) {
        oldStyle = Prompt::style();
        Prompt::setStyle(*style_);
    }

    auto result = Prompt::askChoice(question_, choices_, defaultIndex_);

    if (hasCustomStyle)
        Prompt::setStyle(oldStyle);
    return result;
}

std::vector<size_t> ChoiceBuilder::askMulti() {
    PromptStyle oldStyle;
    bool hasCustomStyle = style_.has_value();
    if (hasCustomStyle) {
        oldStyle = Prompt::style();
        Prompt::setStyle(*style_);
    }

    auto result = Prompt::askMultiChoice(question_, choices_);

    if (hasCustomStyle)
        Prompt::setStyle(oldStyle);
    return result;
}

IntPromptBuilder &IntPromptBuilder::question(const std::string &q) {
    question_ = q;
    return *this;
}

IntPromptBuilder &IntPromptBuilder::defaultValue(int v) {
    defaultValue_ = v;
    return *this;
}

IntPromptBuilder &IntPromptBuilder::range(int min, int max) {
    min_ = min;
    max_ = max;
    return *this;
}

IntPromptBuilder &IntPromptBuilder::min(int m) {
    min_ = m;
    return *this;
}

IntPromptBuilder &IntPromptBuilder::max(int m) {
    max_ = m;
    return *this;
}

IntPromptBuilder &IntPromptBuilder::validator(Validator<int> v,
                                              const std::string &errorMsg) {
    validator_ = std::move(v);
    errorMessage_ = errorMsg;
    return *this;
}

IntPromptBuilder &IntPromptBuilder::maxAttempts(int n) {
    maxAttempts_ = n;
    return *this;
}

std::optional<int> IntPromptBuilder::ask() {
    for (int attempt = 0; attempt < maxAttempts_; ++attempt) {
        auto result = Prompt::askInt(question_, defaultValue_, min_, max_, 1);

        if (!result) {
            return std::nullopt;
        }

        if (validator_ && !validator_(*result)) {
            Prompt::printError(errorMessage_.empty() ? "Validation failed"
                                                     : errorMessage_);
            continue;
        }

        return result;
    }

    return std::nullopt;
}

int IntPromptBuilder::askOr(int fallback) {
    auto result = ask();
    return result.value_or(fallback);
}

} // namespace termtools
