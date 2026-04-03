#ifndef TERMTOOLS_LOGGER_H
#define TERMTOOLS_LOGGER_H

#include "termtools/core/formatting/color.h"
#include "termtools/core/output/console.h"

#include <cstdint>
#include <string>

namespace termtools {

enum class LogLevel : uint8_t { Debug, Info, Warning, Error, Critical };

class Logger {
  public:
    explicit Logger(Console &console = Console::getStdout());

    void setLevel(LogLevel level);
    void setFormat(const std::string &format);
    void setShowTime(bool show);
    void setShowLevel(bool show);
    void setShowLocation(bool show);

    void debug(const std::string &message);
    void info(const std::string &message);
    void warning(const std::string &message);
    void error(const std::string &message);
    void critical(const std::string &message);

    void log(LogLevel level, const std::string &message);

    static Logger &getLogger();

  private:
    Console &console_;
    LogLevel level_;
    std::string format_;
    bool showTime_;
    bool showLevel_;
    bool showLocation_;

    std::string levelToString(LogLevel level) const;
    Color levelToColor(LogLevel level) const;
    std::string formatMessage(LogLevel level, const std::string &message) const;
};

} 

#endif
