#ifndef TERMTOOLS_PROMPT_H
#define TERMTOOLS_PROMPT_H

#include <string>
#include <vector>
#include <functional>
#include <optional>
#include <limits>
#include <any>

namespace termtools {

template<typename T>
using Validator = std::function<bool(const T&)>;

template<typename T>
using Parser = std::function<std::optional<T>(const std::string&)>;

struct PromptStyle {
    std::string questionColor = "cyan";
    std::string defaultValueColor = "dim";
    std::string errorColor = "red";
    std::string hintColor = "yellow";
    std::string promptSuffix = ": ";
    std::string defaultFormat = "[{}]";
    bool showHints = true;
};

class PromptResult {
public:
    enum class Status {
        Ok,
        Cancelled,
        Timeout,
        Invalid
    };
    
    PromptResult() : status_(Status::Invalid) {}
    explicit PromptResult(std::string value) : status_(Status::Ok), value_(std::move(value)) {}
    PromptResult(Status status) : status_(status) {}
    
    bool ok() const { return status_ == Status::Ok; }
    bool cancelled() const { return status_ == Status::Cancelled; }
    Status status() const { return status_; }
    
    const std::string& value() const { return value_; }
    const std::string& operator*() const { return value_; }
    operator bool() const { return ok(); }
    
private:
    Status status_;
    std::string value_;
};

class Prompt {
public:
    static PromptStyle& style();
    static void setStyle(const PromptStyle& style);
    
    static PromptResult ask(
        const std::string& question,
        const std::optional<std::string>& defaultValue = std::nullopt
    );
    
    static bool confirm(
        const std::string& question,
        std::optional<bool> defaultValue = std::nullopt
    );
    
    static std::optional<int> askInt(
        const std::string& question,
        std::optional<int> defaultValue = std::nullopt,
        int min = std::numeric_limits<int>::min(),
        int max = std::numeric_limits<int>::max(),
        int maxAttempts = 3
    );
    
    static std::optional<double> askDouble(
        const std::string& question,
        std::optional<double> defaultValue = std::nullopt,
        double min = -std::numeric_limits<double>::infinity(),
        double max = std::numeric_limits<double>::infinity(),
        int maxAttempts = 3
    );
    
    static std::string askPassword(
        const std::string& question,
        bool mask = true,
        char maskChar = '*'
    );
    
    static std::optional<size_t> askChoice(
        const std::string& question,
        const std::vector<std::string>& choices,
        std::optional<size_t> defaultIndex = std::nullopt
    );
    
    static std::string askChoiceValue(
        const std::string& question,
        const std::vector<std::string>& choices,
        const std::string& defaultValue = ""
    );
    
    static std::vector<size_t> askMultiChoice(
        const std::string& question,
        const std::vector<std::string>& choices
    );
    
    static std::vector<std::string> askMultiChoiceValues(
        const std::string& question,
        const std::vector<std::string>& choices
    );
    
    template<typename T>
    static std::optional<T> askWithParser(
        const std::string& question,
        Parser<T> parser,
        Validator<T> validator = nullptr,
        std::optional<T> defaultValue = std::nullopt,
        int maxAttempts = 3
    );
    
    static std::string askWithValidation(
        const std::string& question,
        Validator<std::string> validator,
        const std::string& errorMessage = "Invalid input",
        std::optional<std::string> defaultValue = std::nullopt,
        int maxAttempts = 3
    );

private:
    static PromptStyle& getMutableStyle();
    static std::string readLine();
    static std::string readLineMasked(char maskChar);
    static void printQuestion(const std::string& question, const std::string& suffix = "");
    static void printError(const std::string& message);
    static void printHint(const std::string& hint);
    
    friend class PromptBuilder;
    friend class IntPromptBuilder;
    friend class ChoiceBuilder;
};

class PromptBuilder {
public:
    PromptBuilder() = default;
    
    PromptBuilder& question(const std::string& q);
    PromptBuilder& defaultValue(const std::string& value);
    PromptBuilder& validator(Validator<std::string> v, const std::string& errorMsg = "");
    PromptBuilder& hint(const std::string& h);
    PromptBuilder& required(bool r = true);
    PromptBuilder& maxAttempts(int n);
    PromptBuilder& style(const PromptStyle& s);
    
    PromptResult ask();
    std::string askOr(const std::string& fallback);
    
private:
    std::string question_;
    std::optional<std::string> defaultValue_;
    Validator<std::string> validator_;
    std::string errorMessage_;
    std::string hint_;
    bool required_ = false;
    int maxAttempts_ = 3;
    std::optional<PromptStyle> style_;
};

class ChoiceBuilder {
public:
    ChoiceBuilder() = default;
    
    ChoiceBuilder& question(const std::string& q);
    ChoiceBuilder& choice(const std::string& c);
    ChoiceBuilder& choices(const std::vector<std::string>& cs);
    ChoiceBuilder& defaultIndex(size_t idx);
    ChoiceBuilder& allowMulti(bool allow = true);
    ChoiceBuilder& style(const PromptStyle& s);
    
    std::optional<size_t> ask();
    std::vector<size_t> askMulti();
    
private:
    std::string question_;
    std::vector<std::string> choices_;
    std::optional<size_t> defaultIndex_;
    bool allowMulti_ = false;
    std::optional<PromptStyle> style_;
};

class IntPromptBuilder {
public:
    IntPromptBuilder() = default;
    
    IntPromptBuilder& question(const std::string& q);
    IntPromptBuilder& defaultValue(int v);
    IntPromptBuilder& range(int min, int max);
    IntPromptBuilder& min(int m);
    IntPromptBuilder& max(int m);
    IntPromptBuilder& validator(Validator<int> v, const std::string& errorMsg = "");
    IntPromptBuilder& maxAttempts(int n);
    
    std::optional<int> ask();
    int askOr(int fallback);
    
private:
    std::string question_;
    std::optional<int> defaultValue_;
    int min_ = std::numeric_limits<int>::min();
    int max_ = std::numeric_limits<int>::max();
    Validator<int> validator_;
    std::string errorMessage_;
    int maxAttempts_ = 3;
};

template<typename T>
std::optional<T> Prompt::askWithParser(
    const std::string& question,
    Parser<T> parser,
    Validator<T> validator,
    std::optional<T> defaultValue,
    int maxAttempts)
{
    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        auto result = ask(question, 
            defaultValue ? std::optional<std::string>(std::to_string(*defaultValue)) : std::nullopt);
        
        if (!result.ok()) {
            return std::nullopt;
        }
        
        const std::string& input = result.value();
        
        if (input.empty() && defaultValue) {
            return defaultValue;
        }
        
        auto parsed = parser(input);
        if (!parsed) {
            printError("Invalid format");
            continue;
        }
        
        if (validator && !validator(*parsed)) {
            printError("Validation failed");
            continue;
        }
        
        return parsed;
    }
    
    return std::nullopt;
}

}

#endif
