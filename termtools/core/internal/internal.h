#ifndef TERMTOOLS_INTERNAL_H
#define TERMTOOLS_INTERNAL_H

#include "types.h"
#include "ansi.h"
#include <string>
#include <vector>
#include "termtools.h"

namespace termtools {
namespace internal {

struct TerminalInfo {
    int width;
    int height;
    bool supportsColor;
    bool supportsUnicode;
    bool isTTY;
};

TerminalInfo getTerminalInfo();
int getTerminalWidth();
int getTerminalHeight();
bool isColorSupported();
bool isUnicodeSupported();
bool isTTY();

std::string formatAnsi(const char* format, ...);
std::string buildAnsiCode(const std::vector<int>& codes);

std::string stripAnsiCodes(const std::string& text);
int measureTextWidth(const std::string& text);
int measureUtf8Width(const std::string& text);

std::string wrapText(const std::string& text, int width);
std::string truncateText(const std::string& text, int width, const std::string& suffix = "...");
std::string padText(const std::string& text, int width, Justify justify = Justify::Left, char fill = ' ');

std::string getCurrentTime(const std::string& format = "%Y-%m-%d %H:%M:%S");
std::string formatDuration(double seconds);

}
}

#endif
