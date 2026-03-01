#ifndef TERMTOOLS_PROGRESS_H
#define TERMTOOLS_PROGRESS_H

#include "termtools/color/color.h"
#include "termtools/core/console.h"

#include <chrono>
#include <cstdint>
#include <memory>
#include <string>

namespace termtools {

enum class SpinnerStyle : uint8_t { Dots, Line, Dots12, Arrow, SimpleDots };

class ProgressBar {
  public:
    ProgressBar(int total, const std::string &description = "",
                Console &console = Console::getStdout());
    ~ProgressBar();

    void update(int advance = 1);
    void setProgress(int current);
    void setDescription(const std::string &description);
    void setStyle(SpinnerStyle style);
    void setColor(const Color &color);

    void setCompletedPrefix(const std::string &prefix);
    void setCompletedSuffix(const std::string &suffix);
    void setInProgressPrefix(const std::string &prefix);
    void setInProgressSuffix(const std::string &suffix);

    void start();
    void finish();
    void reset();

    void refresh();

    int getCurrent() const;
    int getTotal() const;
    double getPercentage() const;

  private:
    class BufferingGuard;

    int total_;
    int current_;
    std::string description_;
    SpinnerStyle spinnerStyle_;
    Color color_;
    std::string completedPrefix_;
    std::string completedSuffix_;
    std::string inProgressPrefix_;
    std::string inProgressSuffix_;
    bool started_;
    std::chrono::steady_clock::time_point startTime_;
    Console &console_;
    std::unique_ptr<BufferingGuard> bufferingGuard_;

    std::string getSpinner() const;
    std::string render() const;
    std::string formatTime(double seconds) const;
};

} 

#endif
