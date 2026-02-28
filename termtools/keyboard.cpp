#include "../termtools.h"
#include "internal.h"

#if defined(_WIN32)
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#endif

#include <atomic>
#include <thread>

namespace termtools {

namespace {
    static std::atomic<bool> rawModeEnabled(false);
    static std::atomic<bool> keyLoopRunning(false);
    static Keyboard::KeyEventCallback globalCallback;
    
#if !defined(_WIN32)
    static struct termios originalTermios;
    static bool termiosSaved = false;
#endif
}

void Keyboard::setRawMode(bool enable) {
#if defined(_WIN32)
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(hStdin, &mode);
    
    if (enable) {
        mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
        SetConsoleMode(hStdin, mode);
    } else {
        mode |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
        SetConsoleMode(hStdin, mode);
    }
#else
    if (enable && !rawModeEnabled) {
        if (!termiosSaved) {
            tcgetattr(STDIN_FILENO, &originalTermios);
            termiosSaved = true;
        }
        
        struct termios raw = originalTermios;
        raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag &= ~(OPOST);
        raw.c_cflag |= (CS8);
        raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN] = 0;
        raw.c_cc[VTIME] = 0;
        
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    } else if (!enable && rawModeEnabled && termiosSaved) {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &originalTermios);
    }
#endif
    
    rawModeEnabled = enable;
}

bool Keyboard::isRawModeEnabled() {
    return rawModeEnabled;
}

Keyboard::KeyEvent Keyboard::readKey(bool blocking) {
    KeyEvent event;
    event.key = Key::Unknown;
    event.ch = '\0';
    event.ctrl = false;
    event.alt = false;
    event.shift = false;
    
#if defined(_WIN32)
    if (blocking) {
        while (!_kbhit()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    if (_kbhit()) {
        int ch = _getch();
        
        if (ch == 0 || ch == 224) {
            ch = _getch();
            
            switch (ch) {
                case 72: event.key = Key::Up; break;
                case 80: event.key = Key::Down; break;
                case 75: event.key = Key::Left; break;
                case 77: event.key = Key::Right; break;
                case 71: event.key = Key::Home; break;
                case 79: event.key = Key::End; break;
                case 73: event.key = Key::PageUp; break;
                case 81: event.key = Key::PageDown; break;
                case 83: event.key = Key::Delete; break;
                case 59: event.key = Key::F1; break;
                case 60: event.key = Key::F2; break;
                case 61: event.key = Key::F3; break;
                case 62: event.key = Key::F4; break;
                case 63: event.key = Key::F5; break;
                case 64: event.key = Key::F6; break;
                case 65: event.key = Key::F7; break;
                case 66: event.key = Key::F8; break;
                case 67: event.key = Key::F9; break;
                case 68: event.key = Key::F10; break;
                case 133: event.key = Key::F11; break;
                case 134: event.key = Key::F12; break;
            }
        } else if (ch == 13) {
            event.key = Key::Enter;
        } else if (ch == 27) {
            event.key = Key::Escape;
        } else if (ch == 9) {
            event.key = Key::Tab;
        } else if (ch == 8) {
            event.key = Key::Backspace;
        } else if (ch < 32) {
            event.ctrl = true;
            event.ch = static_cast<char>(ch + 'A' - 1);
        } else {
            event.ch = static_cast<char>(ch);
        }
    }
#else
    if (blocking) {
        fd_set set;
        struct timeval timeout;
        
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        
        while (true) {
            timeout.tv_sec = 0;
            timeout.tv_usec = 10000;
            
            int result = select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout);
            if (result > 0) {
                break;
            }
        }
    }
    
    char buf[16];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    
    if (n > 0) {
        if (buf[0] == 27) {
            if (n == 1) {
                event.key = Key::Escape;
            } else if (n >= 3 && buf[1] == '[') {
                switch (buf[2]) {
                    case 'A': event.key = Key::Up; break;
                    case 'B': event.key = Key::Down; break;
                    case 'C': event.key = Key::Right; break;
                    case 'D': event.key = Key::Left; break;
                    case 'H': event.key = Key::Home; break;
                    case 'F': event.key = Key::End; break;
                    case 'Z': event.key = Key::Tab; event.shift = true; break;
                    case '1':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::Home;
                    }
                    break;
                case '2':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::Unknown;
                    }
                    break;
                case '3':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::Delete;
                    }
                    break;
                case '4':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::End;
                    }
                    break;
                case '5':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::PageUp;
                    }
                    break;
                case '6':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::PageDown;
                    }
                    break;
                case '7':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::Home;
                    }
                    break;
                case '8':
                    if (n >= 4 && buf[3] == '~') {
                        event.key = Key::End;
                    }
                    break;
                default:
                    break;
                }
                
                if (n >= 4 && buf[2] >= '1' && buf[2] <= '2' && buf[3] >= '0' && buf[3] <= '5') {
                    int fn = (buf[2] - '0') * 10 + (buf[3] - '0');
                    if (fn >= 11 && fn <= 24) {
                        event.key = static_cast<Key>(static_cast<int>(Key::F1) + fn - 11);
                    }
                }
            }
        } else if (buf[0] == 13 || buf[0] == 10) {
            event.key = Key::Enter;
        } else if (buf[0] == 9) {
            event.key = Key::Tab;
        } else if (buf[0] == 127 || buf[0] == 8) {
            event.key = Key::Backspace;
        } else if (buf[0] < 32) {
            event.ctrl = true;
            event.ch = static_cast<char>(buf[0] + 'A' - 1);
        } else {
            event.ch = buf[0];
        }
    }
#endif
    
    return event;
}

bool Keyboard::hasKeyInput() {
#if defined(_WIN32)
    return _kbhit() != 0;
#else
    fd_set set;
    struct timeval timeout;
    
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    
    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &timeout) > 0;
#endif
}

void Keyboard::onKeyEvent(KeyEventCallback callback) {
    globalCallback = callback;
}

void Keyboard::startKeyLoop() {
    if (keyLoopRunning) {
        return;
    }
    
    keyLoopRunning = true;
    setRawMode(true);
    
    std::thread([]() {
        while (keyLoopRunning) {
            if (hasKeyInput()) {
                KeyEvent event = readKey(false);
                if (globalCallback) {
                    globalCallback(event);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }).detach();
}

void Keyboard::stopKeyLoop() {
    keyLoopRunning = false;
    setRawMode(false);
}

std::string Keyboard::keyToString(Key key) {
    switch (key) {
        case Key::Unknown: return "Unknown";
        case Key::Up: return "Up";
        case Key::Down: return "Down";
        case Key::Left: return "Left";
        case Key::Right: return "Right";
        case Key::Enter: return "Enter";
        case Key::Escape: return "Escape";
        case Key::Tab: return "Tab";
        case Key::Backspace: return "Backspace";
        case Key::Delete: return "Delete";
        case Key::Home: return "Home";
        case Key::End: return "End";
        case Key::PageUp: return "PageUp";
        case Key::PageDown: return "PageDown";
        case Key::F1: return "F1";
        case Key::F2: return "F2";
        case Key::F3: return "F3";
        case Key::F4: return "F4";
        case Key::F5: return "F5";
        case Key::F6: return "F6";
        case Key::F7: return "F7";
        case Key::F8: return "F8";
        case Key::F9: return "F9";
        case Key::F10: return "F10";
        case Key::F11: return "F11";
        case Key::F12: return "F12";
        default: return "Unknown";
    }
}

}
