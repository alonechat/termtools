#include "../termtools.h"
#include "internal.h"
#include <cstdarg>
#include <cstdio>

namespace termtools {

Console::Console()
    : stream_(&std::cout)
    , colorSupported_(internal::isColorSupported())
    , unicodeSupported_(internal::isUnicodeSupported())
    , encoding_("UTF-8") {}

Console::~Console() {
    flush();
}

void Console::print(const std::string& text) {
    buffer_ << text;
    if (buffer_.str().size() >= BUFFER_SIZE) {
        flush();
    }
}

void Console::print(const Text& text) {
    buffer_ << text.render();
    if (buffer_.str().size() >= BUFFER_SIZE) {
        flush();
    }
}

void Console::print(const char* format, ...) {
    char buffer[4096];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    
    buffer_ << buffer;
    if (buffer_.str().size() >= BUFFER_SIZE) {
        flush();
    }
}

void Console::println(const std::string& text) {
    buffer_ << text << "\n";
    if (buffer_.str().size() >= BUFFER_SIZE) {
        flush();
    }
}

void Console::println(const Text& text) {
    buffer_ << text.render() << "\n";
    if (buffer_.str().size() >= BUFFER_SIZE) {
        flush();
    }
}

template<typename... Args>
void Console::printf(const char* format, Args&&... args) {
    char buffer[4096];
    snprintf(buffer, sizeof(buffer), format, std::forward<Args>(args)...);
    buffer_ << buffer;
    if (buffer_.str().size() >= BUFFER_SIZE) {
        flush();
    }
}

void Console::clear() {
    *stream_ << internal::ANSI_CLEAR_SCREEN << "\033[H";
    stream_->flush();
}

void Console::clearLine() {
    *stream_ << "\r" << internal::ANSI_CLEAR_LINE;
    stream_->flush();
}

void Console::moveCursor(int row, int col) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), internal::ANSI_CURSOR_POSITION, row, col);
    *stream_ << buffer;
    stream_->flush();
}

void Console::moveCursorUp(int lines) {
    if (lines <= 0) return;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), internal::ANSI_CURSOR_UP, lines);
    *stream_ << buffer;
    stream_->flush();
}

void Console::moveCursorDown(int lines) {
    if (lines <= 0) return;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), internal::ANSI_CURSOR_DOWN, lines);
    *stream_ << buffer;
    stream_->flush();
}

void Console::moveCursorLeft(int cols) {
    if (cols <= 0) return;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), internal::ANSI_CURSOR_LEFT, cols);
    *stream_ << buffer;
    stream_->flush();
}

void Console::moveCursorRight(int cols) {
    if (cols <= 0) return;
    char buffer[64];
    snprintf(buffer, sizeof(buffer), internal::ANSI_CURSOR_RIGHT, cols);
    *stream_ << buffer;
    stream_->flush();
}

void Console::showCursor(bool show) {
    *stream_ << (show ? internal::ANSI_CURSOR_SHOW : internal::ANSI_CURSOR_HIDE);
    stream_->flush();
}

void Console::saveCursor() {
    *stream_ << internal::ANSI_CURSOR_SAVE;
    stream_->flush();
}

void Console::restoreCursor() {
    *stream_ << internal::ANSI_CURSOR_RESTORE;
    stream_->flush();
}

int Console::getTerminalWidth() const {
    return internal::getTerminalWidth();
}

int Console::getTerminalHeight() const {
    return internal::getTerminalHeight();
}

void Console::setEncoding(const std::string& encoding) {
    encoding_ = encoding;
}

bool Console::isColorSupported() const {
    return colorSupported_;
}

bool Console::isUnicodeSupported() const {
    return unicodeSupported_;
}

void Console::flush() {
    if (!buffer_.str().empty()) {
        *stream_ << buffer_.str();
        stream_->flush();
        buffer_.str("");
    }
}

Console& Console::getStdout() {
    static Console instance;
    return instance;
}

Console& Console::getStderr() {
    static Console instance;
    instance.stream_ = &std::cerr;
    return instance;
}

}
