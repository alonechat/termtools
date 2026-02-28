#include "../termtools.h"
#include "internal.h"

namespace termtools {

namespace utils {

std::string stripAnsi(const std::string& text) {
    return internal::stripAnsiCodes(text);
}

int measureTextWidth(const std::string& text) {
    return internal::measureTextWidth(text);
}

std::string wrapText(const std::string& text, int width) {
    return internal::wrapText(text, width);
}

std::string truncateText(const std::string& text, int width, const std::string& suffix) {
    return internal::truncateText(text, width, suffix);
}

std::string padText(const std::string& text, int width, Justify justify, char fill) {
    return internal::padText(text, width, justify, fill);
}

std::string getCurrentTime(const std::string& format) {
    return internal::getCurrentTime(format);
}

std::string formatDuration(double seconds) {
    return internal::formatDuration(seconds);
}

bool isTerminal() {
    return internal::isTTY();
}

bool isColorTerminal() {
    return internal::isColorSupported();
}

bool isUnicodeTerminal() {
    return internal::isUnicodeSupported();
}

}

}
