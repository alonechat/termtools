#ifndef TERMTOOLS_INTERNAL_H
#define TERMTOOLS_INTERNAL_H

#include <string>
#include <vector>
#include "termtools.h"

namespace termtools {
namespace internal {

constexpr const char* ANSI_RESET = "\033[0m";
constexpr const char* ANSI_BOLD = "\033[1m";
constexpr const char* ANSI_DIM = "\033[2m";
constexpr const char* ANSI_ITALIC = "\033[3m";
constexpr const char* ANSI_UNDERLINE = "\033[4m";
constexpr const char* ANSI_BLINK = "\033[5m";
constexpr const char* ANSI_REVERSE = "\033[7m";
constexpr const char* ANSI_HIDDEN = "\033[8m";
constexpr const char* ANSI_STRIKETHROUGH = "\033[9m";

constexpr const char* ANSI_FG_BLACK = "\033[30m";
constexpr const char* ANSI_FG_RED = "\033[31m";
constexpr const char* ANSI_FG_GREEN = "\033[32m";
constexpr const char* ANSI_FG_YELLOW = "\033[33m";
constexpr const char* ANSI_FG_BLUE = "\033[34m";
constexpr const char* ANSI_FG_MAGENTA = "\033[35m";
constexpr const char* ANSI_FG_CYAN = "\033[36m";
constexpr const char* ANSI_FG_WHITE = "\033[37m";
constexpr const char* ANSI_FG_BRIGHT_BLACK = "\033[90m";
constexpr const char* ANSI_FG_BRIGHT_RED = "\033[91m";
constexpr const char* ANSI_FG_BRIGHT_GREEN = "\033[92m";
constexpr const char* ANSI_FG_BRIGHT_YELLOW = "\033[93m";
constexpr const char* ANSI_FG_BRIGHT_BLUE = "\033[94m";
constexpr const char* ANSI_FG_BRIGHT_MAGENTA = "\033[95m";
constexpr const char* ANSI_FG_BRIGHT_CYAN = "\033[96m";
constexpr const char* ANSI_FG_BRIGHT_WHITE = "\033[97m";

constexpr const char* ANSI_BG_BLACK = "\033[40m";
constexpr const char* ANSI_BG_RED = "\033[41m";
constexpr const char* ANSI_BG_GREEN = "\033[42m";
constexpr const char* ANSI_BG_YELLOW = "\033[43m";
constexpr const char* ANSI_BG_BLUE = "\033[44m";
constexpr const char* ANSI_BG_MAGENTA = "\033[45m";
constexpr const char* ANSI_BG_CYAN = "\033[46m";
constexpr const char* ANSI_BG_WHITE = "\033[47m";
constexpr const char* ANSI_BG_BRIGHT_BLACK = "\033[100m";
constexpr const char* ANSI_BG_BRIGHT_RED = "\033[101m";
constexpr const char* ANSI_BG_BRIGHT_GREEN = "\033[102m";
constexpr const char* ANSI_BG_BRIGHT_YELLOW = "\033[103m";
constexpr const char* ANSI_BG_BRIGHT_BLUE = "\033[104m";
constexpr const char* ANSI_BG_BRIGHT_MAGENTA = "\033[105m";
constexpr const char* ANSI_BG_BRIGHT_CYAN = "\033[106m";
constexpr const char* ANSI_BG_BRIGHT_WHITE = "\033[107m";

constexpr const char* ANSI_CURSOR_UP = "\033[%dA";
constexpr const char* ANSI_CURSOR_DOWN = "\033[%dB";
constexpr const char* ANSI_CURSOR_LEFT = "\033[%dD";
constexpr const char* ANSI_CURSOR_RIGHT = "\033[%dC";
constexpr const char* ANSI_CURSOR_POSITION = "\033[%d;%dH";
constexpr const char* ANSI_CURSOR_SAVE = "\033[s";
constexpr const char* ANSI_CURSOR_RESTORE = "\033[u";
constexpr const char* ANSI_CURSOR_HIDE = "\033[?25l";
constexpr const char* ANSI_CURSOR_SHOW = "\033[?25h";
constexpr const char* ANSI_CLEAR_LINE = "\033[2K";
constexpr const char* ANSI_CLEAR_SCREEN = "\033[2J";
constexpr const char* ANSI_CLEAR_FROM_CURSOR = "\033[0J";

constexpr const char* BOX_DRAWING_LIGHT = "│─┌┐└┘├┤┬┴┼";
constexpr const char* BOX_DRAWING_DOUBLE = "║═╔╗╚╝╠╣╦╩╬";
constexpr const char* BOX_DRAWING_ROUNDED = "│─╭╮╰╯├┤┬┴┼";
constexpr const char* BOX_DRAWING_HEAVY = "┃━┏┓┗┛┣┫┳┻╋";

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
