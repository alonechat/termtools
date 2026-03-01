#ifndef TERMTOOLS_KEYBOARD_H
#define TERMTOOLS_KEYBOARD_H

#include <cstdint>
#include <functional>

namespace termtools {

class Keyboard {
  public:
    enum class Key : int {
        Unknown = 0,
        Up = 1,
        Down = 2,
        Left = 3,
        Right = 4,
        Enter = 5,
        Escape = 6,
        Tab = 7,
        Backspace = 8,
        Delete = 9,
        Home = 10,
        End = 11,
        PageUp = 12,
        PageDown = 13,
        F1 = 14,
        F2 = 15,
        F3 = 16,
        F4 = 17,
        F5 = 18,
        F6 = 19,
        F7 = 20,
        F8 = 21,
        F9 = 22,
        F10 = 23,
        F11 = 24,
        F12 = 25
    };

    struct KeyEvent {
        Key key;
        char ch;
        bool ctrl;
        bool alt;
        bool shift;
    };

    using KeyEventCallback = std::function<void(const KeyEvent &)>;

    static void setRawMode(bool enable);
    static bool isRawModeEnabled();

    static KeyEvent readKey(bool blocking = true);
    static bool hasKeyInput();

    static void onKeyEvent(KeyEventCallback callback);
    static void startKeyLoop();
    static void stopKeyLoop();

    static std::string keyToString(Key key);
};

} 

#endif
