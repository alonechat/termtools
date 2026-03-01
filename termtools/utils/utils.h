#ifndef TERMTOOLS_UTILS_H
#define TERMTOOLS_UTILS_H

#include <string>

namespace termtools {

enum class Justify : uint8_t { Left, Center, Right };
enum class Overflow : uint8_t { Fold, Crop, Ellipsis, Ignore };

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

} 

} 

#endif
