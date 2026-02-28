#include "../termtools.h"
#include "internal.h"
#include <sstream>
#include <iomanip>

namespace termtools {

namespace {
    const char* SPINNER_DOTS[] = {"⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"};
    const char* SPINNER_LINE[] = {"|", "/", "-", "\\"};
    const char* SPINNER_DOTS12[] = {"⢀⠀", "⡀⠀", "⠄⠀", "⢂⠀", "⡂⠀", "⠅⠀", "⢃⠀", "⡃⠀", "⠍⠀", "⢋⠀", "⡋⠀", "⠍⠁", "⢋⠁", "⡋⠁", "⠍⠉", "⠋⠉", "⠋⠙", "⠙⠚", "⠚⠖", "⠖⠦", "⠦⠤", "⠤⠄", "⠄⠄", "⠀⠄"};
    const char* SPINNER_ARROW[] = {"←", "↖", "↑", "↗", "→", "↘", "↓", "↙"};
    const char* SPINNER_SIMPLE_DOTS[] = {".  ", ".. ", "...", " ..", "  .", "   "};
    
    const int SPINNER_DOTS_SIZE = 10;
    const int SPINNER_LINE_SIZE = 4;
    const int SPINNER_DOTS12_SIZE = 24;
    const int SPINNER_ARROW_SIZE = 8;
    const int SPINNER_SIMPLE_DOTS_SIZE = 6;
}

ProgressBar::ProgressBar(int total, const std::string& description)
    : total_(total)
    , current_(0)
    , description_(description)
    , spinnerStyle_(SpinnerStyle::Dots)
    , color_(ColorName::Green)
    , completedPrefix_("✓ ")
    , completedSuffix_(" completed")
    , inProgressPrefix_(" ")
    , inProgressSuffix_(" in progress")
    , started_(false)
    , console_(Console::getStdout()) {
    startTime_ = std::chrono::steady_clock::now();
}

ProgressBar::~ProgressBar() {
    if (started_) {
        finish();
    }
}

void ProgressBar::update(int advance) {
    current_ += advance;
    if (current_ > total_) {
        current_ = total_;
    }
    refresh();
}

void ProgressBar::setProgress(int current) {
    current_ = current;
    if (current_ > total_) {
        current_ = total_;
    }
    refresh();
}

void ProgressBar::setDescription(const std::string& description) {
    description_ = description;
    refresh();
}

void ProgressBar::setStyle(SpinnerStyle style) {
    spinnerStyle_ = style;
}

void ProgressBar::setColor(const Color& color) {
    color_ = color;
}

void ProgressBar::setCompletedPrefix(const std::string& prefix) {
    completedPrefix_ = prefix;
}

void ProgressBar::setCompletedSuffix(const std::string& suffix) {
    completedSuffix_ = suffix;
}

void ProgressBar::setInProgressPrefix(const std::string& prefix) {
    inProgressPrefix_ = prefix;
}

void ProgressBar::setInProgressSuffix(const std::string& suffix) {
    inProgressSuffix_ = suffix;
}

void ProgressBar::start() {
    started_ = true;
    startTime_ = std::chrono::steady_clock::now();
    refresh();
}

void ProgressBar::finish() {
    current_ = total_;
    refresh();
    console_.println();
    started_ = false;
}

void ProgressBar::reset() {
    current_ = 0;
    startTime_ = std::chrono::steady_clock::now();
    refresh();
}

void ProgressBar::refresh() {
    std::string rendered = render();
    console_.clearLine();
    console_.print("\r" + rendered);
    console_.flush();
}

int ProgressBar::getCurrent() const {
    return current_;
}

int ProgressBar::getTotal() const {
    return total_;
}

double ProgressBar::getPercentage() const {
    if (total_ <= 0) return 0.0;
    return static_cast<double>(current_) / total_ * 100.0;
}

std::string ProgressBar::getSpinner() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime_
    );
    int index = (elapsed.count() / 100) % 10;
    
    switch (spinnerStyle_) {
        case SpinnerStyle::Dots:
            return SPINNER_DOTS[index % SPINNER_DOTS_SIZE];
        case SpinnerStyle::Line:
            return SPINNER_LINE[index % SPINNER_LINE_SIZE];
        case SpinnerStyle::Dots12:
            return SPINNER_DOTS12[index % SPINNER_DOTS12_SIZE];
        case SpinnerStyle::Arrow:
            return SPINNER_ARROW[index % SPINNER_ARROW_SIZE];
        case SpinnerStyle::SimpleDots:
            return SPINNER_SIMPLE_DOTS[index % SPINNER_SIMPLE_DOTS_SIZE];
        default:
            return SPINNER_DOTS[index % SPINNER_DOTS_SIZE];
    }
}

std::string ProgressBar::formatTime(double seconds) const {
    return internal::formatDuration(seconds);
}

std::string ProgressBar::render() const {
    std::ostringstream oss;
    
    int termWidth = console_.getTerminalWidth();
    double percentage = getPercentage();
    
    bool isComplete = (current_ >= total_);
    
    if (!description_.empty()) {
        oss << description_ << " ";
    }
    
    if (isComplete) {
        StyleBuilder style;
        style.color(Color(ColorName::Green)).bold();
        oss << style.toAnsi() << completedPrefix_ << style.toAnsiReset();
    } else {
        oss << getSpinner() << " ";
    }
    
    int barWidth = std::max(10, termWidth - static_cast<int>(oss.str().length()) - 30);
    int filledWidth = static_cast<int>(barWidth * percentage / 100.0);
    
    oss << "[";
    
    if (!isComplete) {
        StyleBuilder barStyle;
        barStyle.color(color_);
        oss << barStyle.toAnsi();
    }
    
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            oss << "█";
        } else if (i == filledWidth && !isComplete) {
            oss << "▌";
        } else {
            oss << " ";
        }
    }
    
    if (!isComplete) {
        oss << StyleBuilder().toAnsiReset();
    }
    
    oss << "] ";
    
    oss << std::fixed << std::setprecision(1) << percentage << "%";
    
    oss << " " << current_ << "/" << total_;
    
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - startTime_
    );
    double elapsedSeconds = elapsed.count();
    
    if (elapsedSeconds > 0 && current_ > 0 && !isComplete) {
        double estimatedTotal = elapsedSeconds / current_ * total_;
        double remaining = estimatedTotal - elapsedSeconds;
        oss << " [" << formatTime(remaining) << " remaining]";
    } else if (isComplete) {
        oss << " [" << formatTime(elapsedSeconds) << " total]";
    }
    
    return oss.str();
}

}
