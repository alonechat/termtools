#ifndef TERMTOOLS_KEYBOARD_H
#define TERMTOOLS_KEYBOARD_H

#include <cstdint>
#include <functional>
#include <chrono>
#include <optional>
#include <mutex>
#include <string>

namespace termtools {

enum class KeyCode : uint16_t {
    Unknown = 0,
    Char = 1,
    Up = 0x0100,
    Down = 0x0101,
    Left = 0x0102,
    Right = 0x0103,
    Enter = 0x0104,
    Escape = 0x0105,
    Tab = 0x0106,
    Backspace = 0x0107,
    Delete = 0x0108,
    Insert = 0x0109,
    Home = 0x010A,
    End = 0x010B,
    PageUp = 0x010C,
    PageDown = 0x010D,
    F1 = 0x0200,
    F2 = 0x0201,
    F3 = 0x0202,
    F4 = 0x0203,
    F5 = 0x0204,
    F6 = 0x0205,
    F7 = 0x0206,
    F8 = 0x0207,
    F9 = 0x0208,
    F10 = 0x0209,
    F11 = 0x020A,
    F12 = 0x020B,
    F13 = 0x020C,
    F14 = 0x020D,
    F15 = 0x020E,
    F16 = 0x020F,
    F17 = 0x0210,
    F18 = 0x0211,
    F19 = 0x0212,
    F20 = 0x0213,
    F21 = 0x0214,
    F22 = 0x0215,
    F23 = 0x0216,
    F24 = 0x0217
};

struct KeyModifiers {
    bool ctrl : 1;
    bool alt : 1;
    bool shift : 1;
    bool meta : 1;
    bool hyper : 1;
    bool super : 1;

    KeyModifiers() : ctrl(false), alt(false), shift(false), meta(false), hyper(false), super(false) {}
    
    bool any() const { return ctrl || alt || shift || meta || hyper || super; }
    bool none() const { return !any(); }
    
    bool operator==(const KeyModifiers& other) const {
        return ctrl == other.ctrl && alt == other.alt && shift == other.shift 
               && meta == other.meta && hyper == other.hyper && super == other.super;
    }
};

struct KeyEvent {
    KeyCode code;
    char32_t character;
    KeyModifiers modifiers;
    
    KeyEvent() : code(KeyCode::Unknown), character(0) {}
    
    KeyEvent(KeyCode c) : code(c), character(0) {}
    
    KeyEvent(char32_t ch) : code(KeyCode::Char), character(ch) {}
    
    bool isCharacter() const { return code == KeyCode::Char && character != 0; }
    bool isPrintable() const;
    bool isSpecialKey() const { return code != KeyCode::Unknown && code != KeyCode::Char; }
    bool hasModifiers() const { return modifiers.any(); }
    
    std::string toString() const;
    std::string toDebugString() const;
};

class Keyboard {
public:
    using KeyEventCallback = std::function<void(const KeyEvent&)>;
    using KeyFilter = std::function<bool(const KeyEvent&)>;
    
    class RawModeGuard {
    public:
        RawModeGuard();
        ~RawModeGuard();
        RawModeGuard(const RawModeGuard&) = delete;
        RawModeGuard& operator=(const RawModeGuard&) = delete;
        RawModeGuard(RawModeGuard&& other) noexcept;
        RawModeGuard& operator=(RawModeGuard&& other) noexcept;
    private:
        bool active_;
    };

    static void enableRawMode();
    static void disableRawMode();
    static bool isRawModeEnabled();
    static RawModeGuard scopeRawMode();
    
    static std::optional<KeyEvent> readKey(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
    static KeyEvent readKeyBlocking();
    static bool hasInput(std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    
    static void setCallback(KeyEventCallback callback);
    static void setFilter(KeyFilter filter);
    static void startEventLoop();
    static void stopEventLoop();
    static bool isEventLoopRunning();
    
    static std::string keyCodeToString(KeyCode code);
    static KeyCode stringToKeyCode(const std::string& str);
    
    static KeyEvent parseEscapeSequence(const std::string& seq);

private:
    static std::mutex& getMutex();
    static KeyEventCallback& getCallback();
    static KeyFilter& getFilter();
    static std::atomic<bool>& getLoopRunning();
    static std::atomic<bool>& getRawModeState();
};

class InputReader {
public:
    InputReader();
    ~InputReader();
    
    InputReader(const InputReader&) = delete;
    InputReader& operator=(const InputReader&) = delete;
    
    std::optional<KeyEvent> read(std::chrono::milliseconds timeout = std::chrono::milliseconds::max());
    KeyEvent readBlocking();
    bool hasInput(std::chrono::milliseconds timeout = std::chrono::milliseconds::zero());
    
    void setEcho(bool enabled);
    void setBuffered(bool enabled);
    
private:
    struct Impl;
    Impl* impl_;
};

}

#endif
