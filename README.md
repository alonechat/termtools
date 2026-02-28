# TermTools

TODO

## Features

TODO

## Requirements

- C++14 or higher
- CMake 3.14+

## Building

```bash
mkdir build
cd build
cmake ..
make
```

### Build Options

- `TERMTOOLS_BUILD_EXAMPLES`: Build example programs (default: ON)
- `TERMTOOLS_BUILD_TESTS`: Build tests (default: ON)
- `TERMTOOLS_INSTALL`: Generate install target (default: ON)

## Quick Start

### Basic Usage

```cpp
#include "termtools.h"

using namespace termtools;

int main() {
    Console console;
    
    // Print colored text
    Text text("Hello, World!");
    text.colored(Color(ColorName::Green));
    console.println(text);
    
    // Print styled text
    Text styled("Bold and underlined");
    styled.styled(StyleBuilder().bold().underline());
    console.println(styled);
    
    return 0;
}
```

### Tables

```cpp
Table table;
table.addColumn("Name").addColumn("Age").addColumn("City");
table.addRow({"Alice", "28", "New York"});
table.addRow({"Bob", "35", "Los Angeles"});
table.setHeaderColor(Color(ColorName::Cyan));
table.print(console);
```

### Progress Bars

```cpp
ProgressBar progress(100, "Processing");
progress.start();

for (int i = 0; i <= 100; ++i) {
    progress.update(1);
    // ... do work ...
}

progress.finish();
```

### Logging

```cpp
Logger logger;
logger.debug("Debug message");
logger.info("Info message");
logger.warning("Warning message");
logger.error("Error message");
logger.critical("Critical message");
```

### Interactive Prompts

```cpp
std::string name = Prompt::ask("What is your name?");
int age = Prompt::askInt("How old are you?", 0, 150);
bool confirm = Prompt::confirm("Continue?", true);
```

### Keyboard Input

```cpp
Keyboard::setRawMode(true);

while (true) {
    Keyboard::KeyEvent event = Keyboard::readKey(true);
    
    if (event.ch == 'q') break;
    
    std::cout << "Key: " << Keyboard::keyToString(event.key) << std::endl;
}

Keyboard::setRawMode(false);
```

### Markup

```cpp
Text markup = Text::fromMarkup(
    "This is [bold]bold[/bold], [red]red[/red], and [underline]underlined[/underline]!"
);
console.println(markup);
```

## API Reference

### Color

```cpp
Color();                                    // Default color
Color(ColorName name);                      // Named color (16 colors)
Color(uint8_t r, uint8_t g, uint8_t b);    // RGB color (True Color)
Color(uint8_t index256);                    // 256-color palette

bool isDefault() const;
bool isNamed() const;
bool isRGB() const;
bool is256() const;

std::string toAnsi(bool foreground = true) const;
```

### StyleBuilder

```cpp
StyleBuilder& bold(bool enable = true);
StyleBuilder& dim(bool enable = true);
StyleBuilder& italic(bool enable = true);
StyleBuilder& underline(bool enable = true);
StyleBuilder& blink(bool enable = true);
StyleBuilder& reverse(bool enable = true);
StyleBuilder& hidden(bool enable = true);
StyleBuilder& strikethrough(bool enable = true);
StyleBuilder& color(const Color& color);
StyleBuilder& bgColor(const Color& color);

std::string toAnsi() const;
std::string toAnsiReset() const;
```

### Text

```cpp
Text();
Text(const std::string& content);
Text(const std::string& content, const Color& color);
Text(const std::string& content, const StyleBuilder& style);

Text& append(const Text& other);
Text& append(const std::string& str);
Text& prepend(const Text& other);
Text& prepend(const std::string& str);

Text& styled(const StyleBuilder& style);
Text& colored(const Color& color);

Text& justify(Justify justify, int width = 0);
Text& align(Justify justify, int width = 0);
Text& padLeft(int width, char fill = ' ');
Text& padRight(int width, char fill = ' ');
Text& truncate(int max_width, Overflow overflow = Overflow::Ellipsis);

std::string plain() const;
std::string render() const;

int width() const;
int length() const;
bool empty() const;

static Text fromMarkup(const std::string& markup);
```

### Console

```cpp
Console();

void print(const std::string& text);
void print(const Text& text);
void println(const std::string& text = "");
void println(const Text& text);

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

bool isColorSupported() const;
bool isUnicodeSupported() const;

void flush();

static Console& getStdout();
static Console& getStderr();
```

### Table

```cpp
Table();
Table(const std::vector<std::string>& headers);

Table& addColumn(const std::string& header);
Table& addRow(const std::vector<std::string>& cells);

Table& setHeaderStyle(const StyleBuilder& style);
Table& setHeaderColor(const Color& color);
Table& setBorderStyle(const StyleBuilder& style);

Table& setShowHeader(bool show);
Table& setShowBorder(bool show);
Table& setShowEdge(bool show);
Table& setPadding(int padding);

Table& setTitle(const std::string& title);
Table& setCaption(const std::string& caption);

std::string render() const;
void print(Console& console) const;
```

### ProgressBar

```cpp
ProgressBar(int total, const std::string& description = "");

void update(int advance = 1);
void setProgress(int current);
void setDescription(const std::string& description);
void setColor(const Color& color);

void start();
void finish();
void reset();
void refresh();

int getCurrent() const;
int getTotal() const;
double getPercentage() const;
```

### Logger

```cpp
Logger();

void setLevel(LogLevel level);
void setShowTime(bool show);
void setShowLevel(bool show);

void debug(const std::string& message);
void info(const std::string& message);
void warning(const std::string& message);
void error(const std::string& message);
void critical(const std::string& message);
void log(LogLevel level, const std::string& message);

static Logger& getLogger();
```

### Prompt

```cpp
static std::string ask(const std::string& question);
static std::string ask(const std::string& question, const std::string& defaultAnswer);

static bool confirm(const std::string& question, bool defaultAnswer = true);

static int askInt(const std::string& question, int min = INT_MIN, int max = INT_MAX);
static int askInt(const std::string& question, int defaultAnswer, int min, int max);

static float askFloat(const std::string& question, float min = -INFINITY, float max = INFINITY);

static std::string askPassword(const std::string& question);

static std::string askChoice(const std::string& question, const std::vector<std::string>& choices);
```

### Keyboard

```cpp
enum class Key { Unknown, Up, Down, Left, Right, Enter, Escape, Tab, Backspace, Delete, Home, End, PageUp, PageDown, F1-F12 };

struct KeyEvent {
    Key key;
    char ch;
    bool ctrl;
    bool alt;
    bool shift;
};

static void setRawMode(bool enable);
static bool isRawModeEnabled();

static KeyEvent readKey(bool blocking = true);
static bool hasKeyInput();

static void onKeyEvent(KeyEventCallback callback);
static void startKeyLoop();
static void stopKeyLoop();

static std::string keyToString(Key key);
```

### Panel

```cpp
Panel(const std::string& title, const Text& content);
Panel(const std::string& title, const std::string& content);

Panel& setBorderStyle(const StyleBuilder& style);
Panel& setTitleColor(const Color& color);
Panel& setPadding(int padding);
Panel& setExpand(bool expand);
Panel& setWidth(int width);

std::string render() const;
void print(Console& console) const;
```

## Examples

Build and run the examples:

```bash
cd build
./examples/demo
./examples/colors_demo
./examples/table_demo
./examples/progress_demo
./examples/logger_demo
./examples/prompt_demo
./examples/keyboard_demo
```

## Performance

TermTools is designed for high performance:

- **Output Buffering**: Reduces system calls by buffering output
- **Efficient Rendering**: Optimized algorithms for table and progress bar rendering
- **Minimal Overhead**: Lightweight design with minimal memory allocations
- **Fast Text Measurement**: Efficient UTF-8 width calculation

## Platform Support

- **Linux**: Full support with optimal performance
- **macOS**: Full support with optimal performance
- **Windows**: Full support via Windows Terminal / Virtual Terminal Processing

## License

MIT License

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.

## Acknowledgments

Inspired by Python's [rich](https://github.com/Textualize/rich) library.
