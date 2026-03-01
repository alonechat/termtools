#include "termtools/progress/progress.h"
#include "../core/internal.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace termtools {

namespace {

struct SpinnerData {
    const char *const *frames;
    int size;
};

constexpr const char *SPINNER_DOTS[] = {"⠋", "⠙", "⠹", "⠸", "⠼",
                                        "⠴", "⠦", "⠧", "⠇", "⠏"};

constexpr const char *SPINNER_LINE[] = {"|", "/", "-", "\\"};

constexpr const char *SPINNER_DOTS12[] = {
    "⢀⠀", "⡀⠀", "⠄⠀", "⢂⠀", "⡂⠀", "⠅⠀", "⢃⠀", "⡃⠀", "⠍⠀", "⢋⠀", "⡋⠀", "⠍⠁",
    "⢋⠁", "⡋⠁", "⠍⠉", "⠋⠉", "⠋⠙", "⠙⠚", "⠚⠖", "⠖⠦", "⠦⠤", "⠤⠄", "⠄⠄", "⠀⠄"};

constexpr const char *SPINNER_ARROW[] = {"←", "↖", "↑", "↗",
                                         "→", "↘", "↓", "↙"};

constexpr const char *SPINNER_SIMPLE_DOTS[] = {".  ", ".. ", "...",
                                               " ..", "  .", "   "};

SpinnerData getSpinnerData(SpinnerStyle style) {
    switch (style) {
    case SpinnerStyle::Dots:
        return {SPINNER_DOTS, 10};
    case SpinnerStyle::Line:
        return {SPINNER_LINE, 4};
    case SpinnerStyle::Dots12:
        return {SPINNER_DOTS12, 24};
    case SpinnerStyle::Arrow:
        return {SPINNER_ARROW, 8};
    case SpinnerStyle::SimpleDots:
        return {SPINNER_SIMPLE_DOTS, 6};
    default:
        return {SPINNER_DOTS, 10};
    }
}

constexpr int SPINNER_FRAME_INTERVAL_MS = 100;

} // namespace

class ProgressBar::BufferingGuard {
  public:
    explicit BufferingGuard(Console &console)
        : console_(console), wasBuffering_(console.isBuffering()) {
        console_.flush();
        console_.setBuffering(false);
    }

    ~BufferingGuard() {
        console_.flush();
        console_.setBuffering(wasBuffering_);
    }

    BufferingGuard(const BufferingGuard &) = delete;
    BufferingGuard &operator=(const BufferingGuard &) = delete;

  private:
    Console &console_;
    bool wasBuffering_;
};

ProgressBar::ProgressBar(int total, const std::string &description,
                         Console &console)
    : total_(std::max(1, total)), current_(0), description_(description),
      spinnerStyle_(SpinnerStyle::Dots), color_(ColorName::Green),
      completedPrefix_("✓ "), completedSuffix_(" completed"),
      inProgressPrefix_(" "), inProgressSuffix_(" in progress"),
      started_(false), console_(console), bufferingGuard_(nullptr) {
    startTime_ = std::chrono::steady_clock::now();
}

ProgressBar::~ProgressBar() {
    if (started_) {
        finish();
    }
}

void ProgressBar::update(int advance) {
    current_ = std::min(current_ + advance, total_);
    refresh();
}

void ProgressBar::setProgress(int current) {
    current_ = std::max(0, std::min(current, total_));
    refresh();
}

void ProgressBar::setDescription(const std::string &description) {
    description_ = description;
    refresh();
}

void ProgressBar::setStyle(SpinnerStyle style) { spinnerStyle_ = style; }

void ProgressBar::setColor(const Color &color) { color_ = color; }

void ProgressBar::setCompletedPrefix(const std::string &prefix) {
    completedPrefix_ = prefix;
}

void ProgressBar::setCompletedSuffix(const std::string &suffix) {
    completedSuffix_ = suffix;
}

void ProgressBar::setInProgressPrefix(const std::string &prefix) {
    inProgressPrefix_ = prefix;
}

void ProgressBar::setInProgressSuffix(const std::string &suffix) {
    inProgressSuffix_ = suffix;
}

void ProgressBar::start() {
    if (started_) {
        return;
    }

    started_ = true;
    startTime_ = std::chrono::steady_clock::now();
    bufferingGuard_ = std::make_unique<BufferingGuard>(console_);
    refresh();
}

void ProgressBar::finish() {
    if (!started_) {
        return;
    }

    current_ = total_;
    refresh();

    console_.print("\n");
    console_.flush();

    bufferingGuard_.reset();
    started_ = false;
}

void ProgressBar::reset() {
    current_ = 0;
    startTime_ = std::chrono::steady_clock::now();
    if (started_) {
        refresh();
    }
}

void ProgressBar::refresh() {
    if (!started_) {
        return;
    }
    std::string rendered = render();
    console_.print("\r");
    console_.print(internal::ANSI_CLEAR_LINE);
    console_.print(rendered);
    console_.flush();
}

int ProgressBar::getCurrent() const { return current_; }

int ProgressBar::getTotal() const { return total_; }

double ProgressBar::getPercentage() const {
    if (total_ <= 0) {
        return 0.0;
    }
    return static_cast<double>(current_) / total_ * 100.0;
}

std::string ProgressBar::getSpinner() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - startTime_);

    SpinnerData data = getSpinnerData(spinnerStyle_);
    int frameIndex = (elapsed.count() / SPINNER_FRAME_INTERVAL_MS) % data.size;

    return data.frames[frameIndex];
}

std::string ProgressBar::formatTime(double seconds) const {
    return internal::formatDuration(seconds);
}

std::string ProgressBar::render() const {
    std::ostringstream oss;

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

    int termWidth = console_.getTerminalWidth();
    int prefixWidth = static_cast<int>(internal::measureTextWidth(oss.str()));

    constexpr int MIN_BAR_WIDTH = 10;
    constexpr int SUFFIX_RESERVE = 35;

    int barWidth =
        std::max(MIN_BAR_WIDTH, termWidth - prefixWidth - SUFFIX_RESERVE);
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
        std::chrono::steady_clock::now() - startTime_);
    double elapsedSeconds = static_cast<double>(elapsed.count());

    if (elapsedSeconds > 0 && current_ > 0) {
        if (isComplete) {
            oss << " [" << formatTime(elapsedSeconds) << " total]";
        } else {
            double rate = static_cast<double>(current_) / elapsedSeconds;
            if (rate > 0) {
                double remaining = (total_ - current_) / rate;
                oss << " [" << formatTime(remaining) << " remaining]";
            }
        }
    }

    return oss.str();
}

} // namespace termtools
