#include "termtools/components/logger/logger.h"
#include "termtools/core/internal/internal.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace termtools {

Logger::Logger(Console &console)
    : console_(console), level_(LogLevel::Info), format_("[{level}] {message}"),
      showTime_(true), showLevel_(true), showLocation_(false) {}

void Logger::setLevel(LogLevel level) { level_ = level; }

void Logger::setFormat(const std::string &format) { format_ = format; }

void Logger::setShowTime(bool show) { showTime_ = show; }

void Logger::setShowLevel(bool show) { showLevel_ = show; }

void Logger::setShowLocation(bool show) { showLocation_ = show; }

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
    case LogLevel::Debug:
        return "DEBUG";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Warning:
        return "WARNING";
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Critical:
        return "CRITICAL";
    default:
        return "UNKNOWN";
    }
}

Color Logger::levelToColor(LogLevel level) const {
    switch (level) {
    case LogLevel::Debug:
        return Color(ColorName::BrightBlack);
    case LogLevel::Info:
        return Color(ColorName::Green);
    case LogLevel::Warning:
        return Color(ColorName::Yellow);
    case LogLevel::Error:
        return Color(ColorName::Red);
    case LogLevel::Critical:
        return Color(255, 0, 0);
    default:
        return Color(ColorName::White);
    }
}

std::string Logger::formatMessage(LogLevel level,
                                  const std::string &message) const {
    std::ostringstream oss;

    if (showTime_) {
        oss << StyleBuilder().dim().toAnsi() << "["
            << internal::getCurrentTime("%H:%M:%S") << "] "
            << StyleBuilder().toAnsiReset();
    }

    if (showLevel_) {
        StyleBuilder levelStyle;
        levelStyle.color(levelToColor(level)).bold();

        std::string levelStr = levelToString(level);

        oss << levelStyle.toAnsi() << "[" << std::setw(8) << std::left
            << levelStr << "] " << levelStyle.toAnsiReset();
    }

    oss << message;

    return oss.str();
}

void Logger::debug(const std::string &message) {
    log(LogLevel::Debug, message);
}

void Logger::info(const std::string &message) { log(LogLevel::Info, message); }

void Logger::warning(const std::string &message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const std::string &message) {
    log(LogLevel::Error, message);
}

void Logger::critical(const std::string &message) {
    log(LogLevel::Critical, message);
}

void Logger::log(LogLevel level, const std::string &message) {
    if (static_cast<int>(level) < static_cast<int>(level_)) {
        return;
    }

    std::string formatted = formatMessage(level, message);
    console_.println(formatted);
    console_.flush();
}

Logger &Logger::getLogger() {
    static Logger instance;
    return instance;
}

} // namespace termtools
