#include "termtools/core/input/keyboard.h"
#include "termtools/core/internal/internal.h"

#if defined(_WIN32)
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <fcntl.h>
#endif

#include <atomic>
#include <thread>
#include <condition_variable>
#include <cstring>
#include <unordered_map>

namespace termtools {

namespace {

std::atomic<bool> g_rawModeEnabled{false};
std::atomic<bool> g_loopRunning{false};
std::mutex g_mutex;
Keyboard::KeyEventCallback g_callback;
Keyboard::KeyFilter g_filter;

#if !defined(_WIN32)
struct TermiosState {
    termios original;
    bool saved = false;
    bool valid = false;
};
TermiosState g_termiosState;
#endif

#if defined(_WIN32)
DWORD g_originalConsoleMode = 0;
bool g_consoleModeSaved = false;
#endif

}

std::mutex& Keyboard::getMutex() { return g_mutex; }
Keyboard::KeyEventCallback& Keyboard::getCallback() { return g_callback; }
Keyboard::KeyFilter& Keyboard::getFilter() { return g_filter; }
std::atomic<bool>& Keyboard::getLoopRunning() { return g_loopRunning; }
std::atomic<bool>& Keyboard::getRawModeState() { return g_rawModeEnabled; }

void Keyboard::enableRawMode() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_rawModeEnabled) return;
    
#if defined(_WIN32)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (!g_consoleModeSaved) {
        GetConsoleMode(hStdin, &g_originalConsoleMode);
        g_consoleModeSaved = true;
    }
    
    DWORD mode = g_originalConsoleMode;
    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    mode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
    SetConsoleMode(hStdin, mode);
#else
    if (!g_termiosState.saved) {
        if (tcgetattr(STDIN_FILENO, &g_termiosState.original) == 0) {
            g_termiosState.saved = true;
            g_termiosState.valid = true;
        }
    }
    
    if (g_termiosState.valid) {
        termios raw = g_termiosState.original;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }
#endif
    
    g_rawModeEnabled = true;
}

void Keyboard::disableRawMode() {
    std::lock_guard<std::mutex> lock(g_mutex);
    if (!g_rawModeEnabled) return;
    
#if defined(_WIN32)
    if (g_consoleModeSaved) {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleMode(hStdin, g_originalConsoleMode);
    }
#else
    if (g_termiosState.valid) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &g_termiosState.original);
    }
#endif
    
    g_rawModeEnabled = false;
}

bool Keyboard::isRawModeEnabled() {
    return g_rawModeEnabled;
}

Keyboard::RawModeGuard Keyboard::scopeRawMode() {
    return RawModeGuard();
}

Keyboard::RawModeGuard::RawModeGuard() : active_(false) {
    enableRawMode();
    active_ = true;
}

Keyboard::RawModeGuard::~RawModeGuard() {
    if (active_) {
        disableRawMode();
    }
}

Keyboard::RawModeGuard::RawModeGuard(RawModeGuard&& other) noexcept : active_(other.active_) {
    other.active_ = false;
}

Keyboard::RawModeGuard& Keyboard::RawModeGuard::operator=(RawModeGuard&& other) noexcept {
    if (this != &other) {
        if (active_) disableRawMode();
        active_ = other.active_;
        other.active_ = false;
    }
    return *this;
}

namespace {

bool waitForInput(std::chrono::milliseconds timeout) {
#if defined(_WIN32)
    if (timeout == std::chrono::milliseconds::max()) {
        while (!_kbhit()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return true;
    }
    
    auto deadline = std::chrono::steady_clock::now() + timeout;
    while (std::chrono::steady_clock::now() < deadline) {
        if (_kbhit()) return true;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    return false;
#else
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    
    timeval tv;
    if (timeout == std::chrono::milliseconds::max()) {
        tv.tv_sec = 0;
        tv.tv_usec = 100000;
        
        while (true) {
            int result = select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv);
            if (result > 0) return true;
            if (result < 0 && errno != EINTR) return false;
            FD_ZERO(&set);
            FD_SET(STDIN_FILENO, &set);
        }
    }
    
    auto secs = std::chrono::duration_cast<std::chrono::seconds>(timeout);
    auto usecs = std::chrono::duration_cast<std::chrono::microseconds>(timeout - secs);
    
    tv.tv_sec = secs.count();
    tv.tv_usec = usecs.count();
    
    int result = select(STDIN_FILENO + 1, &set, nullptr, nullptr, &tv);
    return result > 0;
#endif
}

KeyEvent parseUnixEscapeSequence(const char* buf, size_t len) {
    KeyEvent event;
    
    if (len < 2 || buf[0] != '\x1b') {
        event.code = KeyCode::Escape;
        return event;
    }
    
    if (buf[1] == '[') {
        if (len == 2) {
            event.code = KeyCode::Unknown;
            return event;
        }
        
        size_t paramStart = 2;
        int params[8] = {0};
        int paramCount = 0;
        size_t i = paramStart;
        
        while (i < len && paramCount < 8) {
            if (buf[i] >= '0' && buf[i] <= '9') {
                int val = 0;
                while (i < len && buf[i] >= '0' && buf[i] <= '9') {
                    val = val * 10 + (buf[i] - '0');
                    ++i;
                }
                params[paramCount++] = val;
                if (i < len && buf[i] == ';') ++i;
            } else if (buf[i] == ';') {
                ++i;
            } else {
                break;
            }
        }
        
        if (i >= len) {
            event.code = KeyCode::Unknown;
            return event;
        }
        
        char finalChar = buf[i];
        
        if (paramCount >= 2) {
            int modifier = params[paramCount - 1];
            event.modifiers.shift = (modifier & 1) != 0;
            event.modifiers.alt = (modifier & 2) != 0;
            event.modifiers.ctrl = (modifier & 4) != 0;
            event.modifiers.meta = (modifier & 8) != 0;
            --paramCount;
        }
        
        switch (finalChar) {
            case 'A': event.code = KeyCode::Up; break;
            case 'B': event.code = KeyCode::Down; break;
            case 'C': event.code = KeyCode::Right; break;
            case 'D': event.code = KeyCode::Left; break;
            case 'H': event.code = KeyCode::Home; break;
            case 'F': event.code = KeyCode::End; break;
            case 'Z': 
                event.code = KeyCode::Tab; 
                event.modifiers.shift = true;
                break;
            case '~':
                if (paramCount > 0) {
                    switch (params[0]) {
                        case 1: event.code = KeyCode::Home; break;
                        case 2: event.code = KeyCode::Insert; break;
                        case 3: event.code = KeyCode::Delete; break;
                        case 4: event.code = KeyCode::End; break;
                        case 5: event.code = KeyCode::PageUp; break;
                        case 6: event.code = KeyCode::PageDown; break;
                        case 7: event.code = KeyCode::Home; break;
                        case 8: event.code = KeyCode::End; break;
                        case 11: event.code = KeyCode::F1; break;
                        case 12: event.code = KeyCode::F2; break;
                        case 13: event.code = KeyCode::F3; break;
                        case 14: event.code = KeyCode::F4; break;
                        case 15: event.code = KeyCode::F5; break;
                        case 17: event.code = KeyCode::F6; break;
                        case 18: event.code = KeyCode::F7; break;
                        case 19: event.code = KeyCode::F8; break;
                        case 20: event.code = KeyCode::F9; break;
                        case 21: event.code = KeyCode::F10; break;
                        case 23: event.code = KeyCode::F11; break;
                        case 24: event.code = KeyCode::F12; break;
                        default: event.code = KeyCode::Unknown; break;
                    }
                }
                break;
            case 'P': event.code = KeyCode::F1; break;
            case 'Q': event.code = KeyCode::F2; break;
            case 'R': event.code = KeyCode::F3; break;
            case 'S': event.code = KeyCode::F4; break;
            default:
                event.code = KeyCode::Unknown;
                break;
        }
    } else if (buf[1] == 'O') {
        if (len >= 3) {
            switch (buf[2]) {
                case 'A': event.code = KeyCode::Up; break;
                case 'B': event.code = KeyCode::Down; break;
                case 'C': event.code = KeyCode::Right; break;
                case 'D': event.code = KeyCode::Left; break;
                case 'H': event.code = KeyCode::Home; break;
                case 'F': event.code = KeyCode::End; break;
                case 'P': event.code = KeyCode::F1; break;
                case 'Q': event.code = KeyCode::F2; break;
                case 'R': event.code = KeyCode::F3; break;
                case 'S': event.code = KeyCode::F4; break;
                default: event.code = KeyCode::Unknown; break;
            }
        }
    } else if (len == 1) {
        event.code = KeyCode::Escape;
    } else {
        event.code = KeyCode::Unknown;
    }
    
    return event;
}

KeyEvent parseWindowsKeyEvent(int ch) {
    KeyEvent event;
    
    switch (ch) {
        case 13: 
        case 10: 
            event.code = KeyCode::Enter; 
            break;
        case 9: 
            event.code = KeyCode::Tab; 
            break;
        case 8: 
            event.code = KeyCode::Backspace; 
            break;
        case 27: 
            event.code = KeyCode::Escape; 
            break;
        default:
            if (ch < 32) {
                event.code = KeyCode::Char;
                event.character = static_cast<char32_t>(ch + 'A' - 1);
                event.modifiers.ctrl = true;
            } else if (ch >= 32 && ch < 127) {
                event.code = KeyCode::Char;
                event.character = static_cast<char32_t>(ch);
            } else {
                event.code = KeyCode::Unknown;
            }
            break;
    }
    
    return event;
}

KeyEvent parseWindowsSpecialKey(int ch) {
    KeyEvent event;
    
    switch (ch) {
        case 72: event.code = KeyCode::Up; break;
        case 80: event.code = KeyCode::Down; break;
        case 75: event.code = KeyCode::Left; break;
        case 77: event.code = KeyCode::Right; break;
        case 71: event.code = KeyCode::Home; break;
        case 79: event.code = KeyCode::End; break;
        case 73: event.code = KeyCode::PageUp; break;
        case 81: event.code = KeyCode::PageDown; break;
        case 82: event.code = KeyCode::Insert; break;
        case 83: event.code = KeyCode::Delete; break;
        case 59: event.code = KeyCode::F1; break;
        case 60: event.code = KeyCode::F2; break;
        case 61: event.code = KeyCode::F3; break;
        case 62: event.code = KeyCode::F4; break;
        case 63: event.code = KeyCode::F5; break;
        case 64: event.code = KeyCode::F6; break;
        case 65: event.code = KeyCode::F7; break;
        case 66: event.code = KeyCode::F8; break;
        case 67: event.code = KeyCode::F9; break;
        case 68: event.code = KeyCode::F10; break;
        case 133: event.code = KeyCode::F11; break;
        case 134: event.code = KeyCode::F12; break;
        default: event.code = KeyCode::Unknown; break;
    }
    
    return event;
}

}

std::optional<KeyEvent> Keyboard::readKey(std::chrono::milliseconds timeout) {
    if (!waitForInput(timeout)) {
        return std::nullopt;
    }
    
#if defined(_WIN32)
    int ch = _getch();
    
    if (ch == 0 || ch == 224) {
        ch = _getch();
        return parseWindowsSpecialKey(ch);
    }
    
    return parseWindowsKeyEvent(ch);
#else
    char buf[32];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    
    if (n <= 0) {
        return std::nullopt;
    }
    
    if (buf[0] == '\x1b') {
        return parseUnixEscapeSequence(buf, static_cast<size_t>(n));
    }
    
    KeyEvent event;
    
    switch (static_cast<unsigned char>(buf[0])) {
        case 13:
        case 10:
            event.code = KeyCode::Enter;
            break;
        case 9:
            event.code = KeyCode::Tab;
            break;
        case 127:
        case 8:
            event.code = KeyCode::Backspace;
            break;
        default:
            if (static_cast<unsigned char>(buf[0]) < 32) {
                event.code = KeyCode::Char;
                event.character = static_cast<char32_t>(buf[0] + 'A' - 1);
                event.modifiers.ctrl = true;
            } else if (static_cast<unsigned char>(buf[0]) < 127) {
                event.code = KeyCode::Char;
                event.character = static_cast<char32_t>(static_cast<unsigned char>(buf[0]));
            } else {
                event.code = KeyCode::Char;
                event.character = static_cast<char32_t>(static_cast<unsigned char>(buf[0]));
            }
            break;
    }
    
    return event;
#endif
}

KeyEvent Keyboard::readKeyBlocking() {
    auto result = readKey(std::chrono::milliseconds::max());
    return result.value_or(KeyEvent());
}

bool Keyboard::hasInput(std::chrono::milliseconds timeout) {
    return waitForInput(timeout);
}

void Keyboard::setCallback(KeyEventCallback callback) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_callback = std::move(callback);
}

void Keyboard::setFilter(KeyFilter filter) {
    std::lock_guard<std::mutex> lock(g_mutex);
    g_filter = std::move(filter);
}

void Keyboard::startEventLoop() {
    if (g_loopRunning.exchange(true)) {
        return;
    }
    
    enableRawMode();
    
    std::thread([]() {
        while (g_loopRunning) {
            auto event = readKey(std::chrono::milliseconds(50));
            if (event) {
                std::lock_guard<std::mutex> lock(g_mutex);
                bool shouldDispatch = true;
                if (g_filter) {
                    shouldDispatch = g_filter(*event);
                }
                if (shouldDispatch && g_callback) {
                    g_callback(*event);
                }
            }
        }
    }).detach();
}

void Keyboard::stopEventLoop() {
    g_loopRunning = false;
    disableRawMode();
}

bool Keyboard::isEventLoopRunning() {
    return g_loopRunning;
}

std::string Keyboard::keyCodeToString(KeyCode code) {
    static const std::unordered_map<KeyCode, const char*> keyNames = {
        {KeyCode::Unknown, "Unknown"},
        {KeyCode::Char, "Char"},
        {KeyCode::Up, "Up"},
        {KeyCode::Down, "Down"},
        {KeyCode::Left, "Left"},
        {KeyCode::Right, "Right"},
        {KeyCode::Enter, "Enter"},
        {KeyCode::Escape, "Escape"},
        {KeyCode::Tab, "Tab"},
        {KeyCode::Backspace, "Backspace"},
        {KeyCode::Delete, "Delete"},
        {KeyCode::Insert, "Insert"},
        {KeyCode::Home, "Home"},
        {KeyCode::End, "End"},
        {KeyCode::PageUp, "PageUp"},
        {KeyCode::PageDown, "PageDown"},
        {KeyCode::F1, "F1"},
        {KeyCode::F2, "F2"},
        {KeyCode::F3, "F3"},
        {KeyCode::F4, "F4"},
        {KeyCode::F5, "F5"},
        {KeyCode::F6, "F6"},
        {KeyCode::F7, "F7"},
        {KeyCode::F8, "F8"},
        {KeyCode::F9, "F9"},
        {KeyCode::F10, "F10"},
        {KeyCode::F11, "F11"},
        {KeyCode::F12, "F12"},
        {KeyCode::F13, "F13"},
        {KeyCode::F14, "F14"},
        {KeyCode::F15, "F15"},
        {KeyCode::F16, "F16"},
        {KeyCode::F17, "F17"},
        {KeyCode::F18, "F18"},
        {KeyCode::F19, "F19"},
        {KeyCode::F20, "F20"},
        {KeyCode::F21, "F21"},
        {KeyCode::F22, "F22"},
        {KeyCode::F23, "F23"},
        {KeyCode::F24, "F24"}
    };
    
    auto it = keyNames.find(code);
    return it != keyNames.end() ? it->second : "Unknown";
}

KeyCode Keyboard::stringToKeyCode(const std::string& str) {
    static const std::unordered_map<std::string, KeyCode> nameToKey = {
        {"Unknown", KeyCode::Unknown},
        {"Char", KeyCode::Char},
        {"Up", KeyCode::Up},
        {"Down", KeyCode::Down},
        {"Left", KeyCode::Left},
        {"Right", KeyCode::Right},
        {"Enter", KeyCode::Enter},
        {"Escape", KeyCode::Escape},
        {"Tab", KeyCode::Tab},
        {"Backspace", KeyCode::Backspace},
        {"Delete", KeyCode::Delete},
        {"Insert", KeyCode::Insert},
        {"Home", KeyCode::Home},
        {"End", KeyCode::End},
        {"PageUp", KeyCode::PageUp},
        {"PageDown", KeyCode::PageDown},
        {"F1", KeyCode::F1},
        {"F2", KeyCode::F2},
        {"F3", KeyCode::F3},
        {"F4", KeyCode::F4},
        {"F5", KeyCode::F5},
        {"F6", KeyCode::F6},
        {"F7", KeyCode::F7},
        {"F8", KeyCode::F8},
        {"F9", KeyCode::F9},
        {"F10", KeyCode::F10},
        {"F11", KeyCode::F11},
        {"F12", KeyCode::F12}
    };
    
    auto it = nameToKey.find(str);
    return it != nameToKey.end() ? it->second : KeyCode::Unknown;
}

KeyEvent Keyboard::parseEscapeSequence(const std::string& seq) {
    return parseUnixEscapeSequence(seq.c_str(), seq.length());
}

bool KeyEvent::isPrintable() const {
    if (!isCharacter()) return false;
    if (character < 32) return false;
    if (character == 127) return false;
    return true;
}

std::string KeyEvent::toString() const {
    if (isCharacter()) {
        std::string result;
        char32_t ch = character;
        if (ch < 0x80) {
            result = static_cast<char>(ch);
        } else if (ch < 0x800) {
            result += static_cast<char>(0xC0 | (ch >> 6));
            result += static_cast<char>(0x80 | (ch & 0x3F));
        } else if (ch < 0x10000) {
            result += static_cast<char>(0xE0 | (ch >> 12));
            result += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (ch & 0x3F));
        } else {
            result += static_cast<char>(0xF0 | (ch >> 18));
            result += static_cast<char>(0x80 | ((ch >> 12) & 0x3F));
            result += static_cast<char>(0x80 | ((ch >> 6) & 0x3F));
            result += static_cast<char>(0x80 | (ch & 0x3F));
        }
        return result;
    }
    return Keyboard::keyCodeToString(code);
}

std::string KeyEvent::toDebugString() const {
    std::string result = Keyboard::keyCodeToString(code);
    
    if (isCharacter()) {
        result += "('" + toString() + "')";
    }
    
    std::string mods;
    if (modifiers.ctrl) mods += "Ctrl+";
    if (modifiers.alt) mods += "Alt+";
    if (modifiers.shift) mods += "Shift+";
    if (modifiers.meta) mods += "Meta+";
    if (modifiers.super) mods += "Super+";
    if (modifiers.hyper) mods += "Hyper+";
    
    if (!mods.empty()) {
        mods.pop_back();
        result += " [" + mods + "]";
    }
    
    return result;
}

struct InputReader::Impl {
    bool echoEnabled = true;
    bool bufferedEnabled = true;
};

InputReader::InputReader() : impl_(new Impl()) {}

InputReader::~InputReader() {
    delete impl_;
}

std::optional<KeyEvent> InputReader::read(std::chrono::milliseconds timeout) {
    return Keyboard::readKey(timeout);
}

KeyEvent InputReader::readBlocking() {
    return Keyboard::readKeyBlocking();
}

bool InputReader::hasInput(std::chrono::milliseconds timeout) {
    return Keyboard::hasInput(timeout);
}

void InputReader::setEcho(bool enabled) {
    impl_->echoEnabled = enabled;
}

void InputReader::setBuffered(bool enabled) {
    impl_->bufferedEnabled = enabled;
}

}
