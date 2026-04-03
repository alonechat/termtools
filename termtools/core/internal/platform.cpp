#include "termtools/core/internal/internal.h"

#if defined(_WIN32)
#include <windows.h>
#include <io.h>
#include <conio.h>
#include <fcntl.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#endif

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>

namespace termtools {
namespace internal {

#if defined(_WIN32)

static HANDLE getStdHandle(int fd) {
    if (fd == STDIN_FILENO) return GetStdHandle(STD_INPUT_HANDLE);
    if (fd == STDOUT_FILENO) return GetStdHandle(STD_OUTPUT_HANDLE);
    if (fd == STDERR_FILENO) return GetStdHandle(STD_ERROR_HANDLE);
    return INVALID_HANDLE_VALUE;
}

static bool enableVirtualTerminalProcessing(HANDLE hConsole) {
    DWORD mode = 0;
    if (!GetConsoleMode(hConsole, &mode)) {
        return false;
    }
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN;
    return SetConsoleMode(hConsole, mode);
}

static bool isVirtualTerminalEnabled(HANDLE hConsole) {
    DWORD mode = 0;
    if (!GetConsoleMode(hConsole, &mode)) {
        return false;
    }
    return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
}

static bool initVirtualTerminal() {
    static bool initialized = false;
    static bool success = false;
    
    if (initialized) {
        return success;
    }
    
    initialized = true;
    
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
    
    success = enableVirtualTerminalProcessing(hStdout);
    if (!success) {
        success = enableVirtualTerminalProcessing(hStderr);
    }
    
    return success;
}

#else

static struct termios orig_termios;
static bool termios_saved = false;

static void disableRawMode() {
    if (termios_saved) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
        termios_saved = false;
    }
}

static void enableRawMode() {
    if (!termios_saved) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        termios_saved = true;
        atexit(disableRawMode);
    }
    
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

#endif

TerminalInfo getTerminalInfo() {
    TerminalInfo info;
    
#if defined(_WIN32)
    initVirtualTerminal();
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    
    if (GetConsoleScreenBufferInfo(hStdout, &csbi)) {
        info.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        info.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    } else {
        info.width = 80;
        info.height = 24;
    }
    
    info.supportsColor = isVirtualTerminalEnabled(hStdout);
    info.supportsUnicode = true;
    info.isTTY = _isatty(_fileno(stdout)) != 0;
#else
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        info.width = ws.ws_col;
        info.height = ws.ws_row;
    } else {
        info.width = 80;
        info.height = 24;
    }
    
    const char* term = getenv("TERM");
    const char* colorterm = getenv("COLORTERM");
    
    info.supportsColor = (term != nullptr && strcmp(term, "dumb") != 0) ||
                         (colorterm != nullptr);
    info.supportsUnicode = true;
    info.isTTY = isatty(STDOUT_FILENO) != 0;
#endif
    
    return info;
}

int getTerminalWidth() {
    return getTerminalInfo().width;
}

int getTerminalHeight() {
    return getTerminalInfo().height;
}

bool isColorSupported() {
    return getTerminalInfo().supportsColor;
}

bool isUnicodeSupported() {
    return getTerminalInfo().supportsUnicode;
}

bool isTTY() {
    return getTerminalInfo().isTTY;
}

std::string buildAnsiCode(const std::vector<int>& codes) {
    if (codes.empty()) {
        return std::string(ANSI_RESET);
    }
    
    std::string result = "\033[";
    for (size_t i = 0; i < codes.size(); ++i) {
        if (i > 0) {
            result += ";";
        }
        result += std::to_string(codes[i]);
    }
    result += "m";
    
    return result;
}

std::string formatAnsi(const char* format, ...) {
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::string(buffer);
}

std::string stripAnsiCodes(const std::string& text) {
    std::string result;
    result.reserve(text.size());
    
    bool inEscape = false;
    for (char c : text) {
        if (c == '\033') {
            inEscape = true;
        } else if (inEscape && (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')) {
            inEscape = false;
        } else if (!inEscape) {
            result += c;
        }
    }
    
    return result;
}

static bool isUtf8ContinuationByte(unsigned char c) {
    return (c & 0xC0) == 0x80;
}

static int getUtf8CharWidth(unsigned char firstByte) {
    if ((firstByte & 0x80) == 0x00) {
        return 1;
    } else if ((firstByte & 0xE0) == 0xC0) {
        return 2;
    } else if ((firstByte & 0xF0) == 0xE0) {
        return 3;
    } else if ((firstByte & 0xF8) == 0xF0) {
        return 4;
    }
    return 1;
}

static int getDisplayWidth(unsigned char firstByte, const std::string& text, size_t pos) {
    if ((firstByte & 0x80) == 0x00) {
        return 1;
    } else if ((firstByte & 0xE0) == 0xC0) {
        return 1;
    } else if ((firstByte & 0xF0) == 0xE0) {
        if (firstByte >= 0xE4 && firstByte <= 0xE9) {
            return 2;
        } else if (firstByte >= 0xEA && firstByte <= 0xED) {
            return 2;
        } else if (firstByte == 0xEF) {
            return 1;
        }
        return 2;
    } else if ((firstByte & 0xF8) == 0xF0) {
        if (pos + 3 < text.size()) {
            uint32_t codepoint = 
                ((uint32_t)(firstByte & 0x07) << 18) |
                ((uint32_t)(text[pos + 1] & 0x3F) << 12) |
                ((uint32_t)(text[pos + 2] & 0x3F) << 6) |
                ((uint32_t)(text[pos + 3] & 0x3F));
            
            if ((codepoint >= 0x1F600 && codepoint <= 0x1F64F) ||
                (codepoint >= 0x1F300 && codepoint <= 0x1F5FF) ||
                (codepoint >= 0x1F680 && codepoint <= 0x1F6FF) ||
                (codepoint >= 0x1F1E0 && codepoint <= 0x1F1FF) ||
                (codepoint >= 0x2600 && codepoint <= 0x26FF) ||
                (codepoint >= 0x2700 && codepoint <= 0x27BF) ||
                (codepoint >= 0x1F900 && codepoint <= 0x1F9FF) ||
                (codepoint >= 0x1FA00 && codepoint <= 0x1FA6F) ||
                (codepoint >= 0x1FA70 && codepoint <= 0x1FAFF)) {
                return 2;
            }
        }
        return 2;
    }
    return 1;
}

int measureUtf8Width(const std::string& text) {
    int width = 0;
    size_t i = 0;
    
    while (i < text.size()) {
        unsigned char c = text[i];
        
        if (c == '\033') {
            while (i < text.size() && text[i] != 'm' && text[i] != 'K') {
                i++;
            }
            if (i < text.size()) {
                i++;
            }
            continue;
        }
        
        if (isUtf8ContinuationByte(c)) {
            i++;
            continue;
        }
        
        int charLen = getUtf8CharWidth(c);
        int displayWidth = getDisplayWidth(c, text, i);
        
        width += displayWidth;
        i += charLen;
    }
    
    return width;
}

int measureTextWidth(const std::string& text) {
    return measureUtf8Width(text);
}

std::string wrapText(const std::string& text, int width) {
    if (width <= 0) {
        return text;
    }
    
    std::string result;
    std::string line;
    int currentWidth = 0;
    
    size_t i = 0;
    while (i < text.size()) {
        if (text[i] == '\n') {
            result += line + "\n";
            line.clear();
            currentWidth = 0;
            i++;
            continue;
        }
        
        unsigned char c = text[i];
        
        if (c == '\033') {
            while (i < text.size() && text[i] != 'm' && text[i] != 'K') {
                line += text[i];
                i++;
            }
            if (i < text.size()) {
                line += text[i];
                i++;
            }
            continue;
        }
        
        if (isUtf8ContinuationByte(c)) {
            line += c;
            i++;
            continue;
        }
        
        int charLen = getUtf8CharWidth(c);
        int displayWidth = getDisplayWidth(c, text, i);
        
        if (currentWidth + displayWidth > width && !line.empty()) {
            result += line + "\n";
            line.clear();
            currentWidth = 0;
        }
        
        for (int j = 0; j < charLen && i < text.size(); ++j) {
            line += text[i++];
        }
        currentWidth += displayWidth;
    }
    
    if (!line.empty()) {
        result += line;
    }
    
    return result;
}

std::string truncateText(const std::string& text, int width, const std::string& suffix) {
    if (width <= 0) {
        return "";
    }
    
    int suffixWidth = measureTextWidth(suffix);
    if (suffixWidth >= width) {
        return "";
    }
    
    int availableWidth = width - suffixWidth;
    std::string result;
    int currentWidth = 0;
    
    size_t i = 0;
    while (i < text.size() && currentWidth < availableWidth) {
        if (text[i] == '\033') {
            while (i < text.size() && text[i] != 'm' && text[i] != 'K') {
                result += text[i];
                i++;
            }
            if (i < text.size()) {
                result += text[i];
                i++;
            }
            continue;
        }
        
        unsigned char c = text[i];
        
        if (isUtf8ContinuationByte(c)) {
            result += c;
            i++;
            continue;
        }
        
        int charLen = getUtf8CharWidth(c);
        int displayWidth = getDisplayWidth(c, text, i);
        
        if (currentWidth + displayWidth > availableWidth) {
            break;
        }
        
        for (int j = 0; j < charLen && i < text.size(); ++j) {
            result += text[i++];
        }
        currentWidth += displayWidth;
    }
    
    return result + suffix;
}

std::string padText(const std::string& text, int width, Justify justify, char fill) {
    int textWidth = measureTextWidth(text);
    
    if (textWidth >= width) {
        return text;
    }
    
    int padding = width - textWidth;
    std::string result;
    
    switch (justify) {
        case Justify::Left:
            result = text + std::string(padding, fill);
            break;
        case Justify::Right:
            result = std::string(padding, fill) + text;
            break;
        case Justify::Center:
            int leftPadding = padding / 2;
            int rightPadding = padding - leftPadding;
            result = std::string(leftPadding, fill) + text + std::string(rightPadding, fill);
            break;
    }
    
    return result;
}

std::string getCurrentTime(const std::string& format) {
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    
    char buffer[64];
    strftime(buffer, sizeof(buffer), format.c_str(), tm_info);
    
    return std::string(buffer);
}

std::string formatDuration(double seconds) {
    char buffer[64];
    
    if (seconds < 1.0) {
        snprintf(buffer, sizeof(buffer), "%.0fms", seconds * 1000);
    } else if (seconds < 60.0) {
        snprintf(buffer, sizeof(buffer), "%.1fs", seconds);
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60);
        int secs = static_cast<int>(seconds) % 60;
        snprintf(buffer, sizeof(buffer), "%dm %ds", minutes, secs);
    } else {
        int hours = static_cast<int>(seconds / 3600);
        int minutes = static_cast<int>((seconds - hours * 3600) / 60);
        snprintf(buffer, sizeof(buffer), "%dh %dm", hours, minutes);
    }
    
    return std::string(buffer);
}

}
}
