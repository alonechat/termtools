#ifndef TERMTOOLS_H
#define TERMTOOLS_H

#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace termtools {

enum class ColorName : uint8_t {
    Black = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
    BrightBlack = 8,
    BrightRed = 9,
    BrightGreen = 10,
    BrightYellow = 11,
    BrightBlue = 12,
    BrightMagenta = 13,
    BrightCyan = 14,
    BrightWhite = 15
};

enum class Style : uint8_t {
    Bold = 1,
    Dim = 2,
    Italic = 3,
    Underline = 4,
    Blink = 5,
    Reverse = 7,
    Hidden = 8,
    Strikethrough = 9
};

enum class Justify : uint8_t { Left, Center, Right };

enum class Overflow : uint8_t { Fold, Crop, Ellipsis, Ignore };

enum class LogLevel : uint8_t { Debug, Info, Warning, Error, Critical };

enum class SpinnerStyle : uint8_t { Dots, Line, Dots12, Arrow, SimpleDots };

class Color {
  public:
    Color();
    explicit Color(ColorName name);
    Color(uint8_t r, uint8_t g, uint8_t b);
    explicit Color(uint8_t index256);

    bool isDefault() const;
    bool isNamed() const;
    bool isRGB() const;
    bool is256() const;

    ColorName getName() const;
    std::tuple<uint8_t, uint8_t, uint8_t> getRGB() const;
    uint8_t getIndex256() const;

    std::string toAnsi(bool foreground = true) const;

  private:
    enum class Type : uint8_t { Default, Named, RGB, Index256 };
    Type type_;
    ColorName name_;
    uint8_t r_, g_, b_;
    uint8_t index256_;
};

class StyleBuilder {
  public:
    StyleBuilder();
    StyleBuilder &bold(bool enable = true);
    StyleBuilder &dim(bool enable = true);
    StyleBuilder &italic(bool enable = true);
    StyleBuilder &underline(bool enable = true);
    StyleBuilder &blink(bool enable = true);
    StyleBuilder &reverse(bool enable = true);
    StyleBuilder &hidden(bool enable = true);
    StyleBuilder &strikethrough(bool enable = true);
    StyleBuilder &color(const Color &color);
    StyleBuilder &bgColor(const Color &color);

    std::string toAnsi() const;
    std::string toAnsiReset() const;

  private:
    bool bold_;
    bool dim_;
    bool italic_;
    bool underline_;
    bool blink_;
    bool reverse_;
    bool hidden_;
    bool strikethrough_;
    Color fgColor_;
    Color bgColor_;
};

class Text {
  public:
    Text();
    explicit Text(const std::string &content);
    Text(const std::string &content, const Color &color);
    Text(const std::string &content, const StyleBuilder &style);

    Text &append(const Text &other);
    Text &append(const std::string &str);
    Text &append(const std::string &str, const Color &color);
    Text &append(const std::string &str, const StyleBuilder &style);

    Text &prepend(const Text &other);
    Text &prepend(const std::string &str);

    Text &styled(const StyleBuilder &style);
    Text &colored(const Color &color);

    Text &justify(Justify justify, int width = 0);
    Text &align(Justify justify, int width = 0);

    Text &padLeft(int width, char fill = ' ');
    Text &padRight(int width, char fill = ' ');

    Text &truncate(int max_width, Overflow overflow = Overflow::Ellipsis);

    std::string plain() const;
    std::string render() const;

    int width() const;
    int length() const;
    bool empty() const;

    static Text fromMarkup(const std::string &markup);

  private:
    struct Segment {
        std::string content;
        StyleBuilder style;
    };
    std::vector<Segment> segments_;
    int calculateWidth() const;
};

class Console {
  public:
    Console();
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

    void flush();

    static Console &getStdout();
    static Console &getStderr();

  private:
    std::ostream *stream_;
    bool colorSupported_;
    bool unicodeSupported_;
    std::string encoding_;
    std::ostringstream buffer_;
    static constexpr size_t BUFFER_SIZE = 8192;
};

class Table {
  public:
    struct Column {
        std::string header;
        Justify justify = Justify::Left;
        int width = 0;
        bool no_wrap = false;
        Color headerColor;
        StyleBuilder headerStyle;
    };

    Table();
    explicit Table(const std::vector<std::string> &headers);

    Table &addColumn(const std::string &header);
    Table &addColumn(const Column &column);

    Table &addRow(const std::vector<std::string> &cells);
    Table &addRow(const std::vector<Text> &cells);

    Table &setHeaderStyle(const StyleBuilder &style);
    Table &setHeaderColor(const Color &color);
    Table &setBorderStyle(const StyleBuilder &style);
    Table &setRowStyle(const StyleBuilder &style);

    Table &setShowHeader(bool show);
    Table &setShowBorder(bool show);
    Table &setShowEdge(bool show);
    Table &setPadding(int padding);

    Table &setTitle(const std::string &title);
    Table &setCaption(const std::string &caption);

    std::string render() const;
    void print(Console &console) const;

    int getColumnCount() const;
    int getRowCount() const;

  private:
    std::vector<Column> columns_;
    std::vector<std::vector<Text>> rows_;
    StyleBuilder headerStyle_;
    StyleBuilder borderStyle_;
    StyleBuilder rowStyle_;
    bool showHeader_ = true;
    bool showBorder_ = true;
    bool showEdge_ = true;
    int padding_ = 1;
    std::string title_;
    std::string caption_;

    std::vector<int> calculateColumnWidths() const;
    std::string renderRow(const std::vector<Text> &cells,
                          const std::vector<int> &widths,
                          bool isHeader = false) const;
    std::string renderSeparator(const std::vector<int> &widths,
                                bool top = false, bool bottom = false) const;
};

class ProgressBar {
  public:
    ProgressBar(int total, const std::string &description = "");
    ~ProgressBar();

    void update(int advance = 1);
    void setProgress(int current);
    void setDescription(const std::string &description);
    void setStyle(SpinnerStyle style);
    void setColor(const Color &color);

    void setCompletedPrefix(const std::string &prefix);
    void setCompletedSuffix(const std::string &suffix);
    void setInProgressPrefix(const std::string &prefix);
    void setInProgressSuffix(const std::string &suffix);

    void start();
    void finish();
    void reset();

    void refresh();

    int getCurrent() const;
    int getTotal() const;
    double getPercentage() const;

  private:
    int total_;
    int current_;
    std::string description_;
    SpinnerStyle spinnerStyle_;
    Color color_;
    std::string completedPrefix_;
    std::string completedSuffix_;
    std::string inProgressPrefix_;
    std::string inProgressSuffix_;
    bool started_;
    std::chrono::steady_clock::time_point startTime_;
    Console &console_;

    std::string getSpinner() const;
    std::string render() const;
    std::string formatTime(double seconds) const;
};

class Status {
  public:
    Status(const std::string &status, const std::string &message = "");
    ~Status();

    void update(const std::string &status, const std::string &message = "");
    void stop();

    void setStatusStyle(const StyleBuilder &style);
    void setMessageStyle(const StyleBuilder &style);

    static Status &start(const std::string &status,
                         const std::string &message = "");

  private:
    std::string status_;
    std::string message_;
    StyleBuilder statusStyle_;
    StyleBuilder messageStyle_;
    Console &console_;
    bool stopped_;
    std::string lastRender_;

    void render();
    void clearLastRender();
};

class Logger {
  public:
    Logger();
    explicit Logger(Console &console);

    void setLevel(LogLevel level);
    void setFormat(const std::string &format);
    void setShowTime(bool show);
    void setShowLevel(bool show);
    void setShowLocation(bool show);

    void debug(const std::string &message);
    void info(const std::string &message);
    void warning(const std::string &message);
    void error(const std::string &message);
    void critical(const std::string &message);

    void log(LogLevel level, const std::string &message);

    static Logger &getLogger();

  private:
    Console &console_;
    LogLevel level_;
    std::string format_;
    bool showTime_;
    bool showLevel_;
    bool showLocation_;

    std::string levelToString(LogLevel level) const;
    Color levelToColor(LogLevel level) const;
    std::string formatMessage(LogLevel level, const std::string &message) const;
};

class Prompt {
  public:
    static std::string ask(const std::string &question);
    static std::string ask(const std::string &question,
                           const std::string &defaultAnswer);

    static bool confirm(const std::string &question, bool defaultAnswer = true);

    static int askInt(const std::string &question, int min, int max);
    static int askInt(const std::string &question, int defaultAnswer, int min,
                      int max);

    static float askFloat(const std::string &question, float min = -INFINITY,
                          float max = INFINITY);
    static float askFloat(const std::string &question, float defaultAnswer,
                          float min = -INFINITY, float max = INFINITY);

    static std::string askPassword(const std::string &question);

    static std::string askChoice(const std::string &question,
                                 const std::vector<std::string> &choices);
    static std::string askChoice(const std::string &question,
                                 const std::vector<std::string> &choices,
                                 const std::string &defaultChoice);

    static std::vector<std::string>
    askMultipleChoice(const std::string &question,
                      const std::vector<std::string> &choices);

  private:
    static std::string readLine();
    static bool readYesNo(bool defaultAnswer);
};

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

class Panel {
  public:
    Panel(const std::string &title, const Text &content);
    Panel(const std::string &title, const std::string &content);

    Panel &setBorderStyle(const StyleBuilder &style);
    Panel &setTitleColor(const Color &color);
    Panel &setPadding(int padding);
    Panel &setExpand(bool expand);
    Panel &setWidth(int width);

    std::string render() const;
    void print(Console &console) const;

  private:
    std::string title_;
    Text content_;
    StyleBuilder borderStyle_;
    Color titleColor_;
    int padding_;
    bool expand_;
    int width_;

    int calculateWidth() const;
    std::string renderBorder(int width, bool top = false,
                             bool bottom = false) const;
};

class Columns {
  public:
    Columns();
    explicit Columns(int columnCount);

    Columns &add(const Text &text);
    Columns &add(const std::string &text);
    Columns &add(const std::vector<Text> &texts);

    Columns &setExpand(bool expand);
    Columns &setPadding(int padding);
    Columns &setEqualWidth(bool equalWidth);

    std::string render() const;
    void print(Console &console) const;

  private:
    std::vector<std::vector<Text>> columns_;
    bool expand_;
    int padding_;
    bool equalWidth_;

    std::vector<int> calculateColumnWidths() const;
};

class Group {
  public:
    Group();
    explicit Group(const std::string &title);

    Group &add(const Text &text);
    Group &add(const std::string &text);
    Group &add(const Panel &panel);
    Group &add(const Table &table);

    Group &setTitle(const std::string &title);
    Group &setBorderStyle(const StyleBuilder &style);

    std::string render() const;
    void print(Console &console) const;

  private:
    std::string title_;
    std::vector<std::function<std::string()>> renderables_;
    StyleBuilder borderStyle_;
};

namespace utils {

std::string stripAnsi(const std::string &text);
int measureTextWidth(const std::string &text);
std::string wrapText(const std::string &text, int width);
std::string truncateText(const std::string &text, int width,
                         const std::string &suffix = "...");
std::string padText(const std::string &text, int width,
                    Justify justify = Justify::Left, char fill = ' ');

std::string getCurrentTime(const std::string &format = "%Y-%m-%d %H:%M:%S");
std::string formatDuration(double seconds);

bool isTerminal();
bool isColorTerminal();
bool isUnicodeTerminal();

} // namespace utils

} // namespace termtools

#endif
