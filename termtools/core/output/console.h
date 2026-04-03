#ifndef TERMTOOLS_CONSOLE_H
#define TERMTOOLS_CONSOLE_H

#include "termtools/core/formatting/color.h"
#include "termtools/core/formatting/text.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace termtools {

class Console {
  public:
    Console();
    explicit Console(std::ostream &stream);
    ~Console();

    void print(const std::string &text);
    void print(const Text &text);
    void print(const char *format, ...);

    void println(const std::string &text = "");
    void println(const Text &text);

    template <typename... Args> void printf(const char *format, Args &&...args);

    void clear();
    void clearLine();
    void moveCursor(int row, int col);
    void moveCursorUp(int lines);
    void moveCursorDown(int lines);
    void moveCursorLeft(int cols);
    void moveCursorRight(int cols);

    void showCursor(bool show = true);
    void saveCursor();
    void restoreCursor();

    int getTerminalWidth() const;
    int getTerminalHeight() const;

    void setEncoding(const std::string &encoding);
    bool isColorSupported() const;
    bool isUnicodeSupported() const;

    void setBuffering(bool enabled);
    bool isBuffering() const;
    void flush();

    static Console &getStdout();
    static Console &getStderr();

  private:
    std::ostream *stream_;
    bool colorSupported_;
    bool unicodeSupported_;
    std::string encoding_;
    std::ostringstream buffer_;
    bool bufferingEnabled_;
    static constexpr size_t BUFFER_SIZE = 8192;
};

} 

#endif
