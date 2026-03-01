#include "termtools.h"

using namespace termtools;

int main() {
    Console console;

    Logger logger(console);

    console.println("=== Logger Demo ===");
    console.println();

    logger.debug("This is a debug message");
    logger.info("This is an info message");
    logger.warning("This is a warning message");
    logger.error("This is an error message");
    logger.critical("This is a critical message");

    console.println();
    console.println("=== Custom Log Level ===");
    console.println();

    logger.setLevel(LogLevel::Warning);
    logger.debug("This debug message won't be shown");
    logger.info("This info message won't be shown");
    logger.warning("This warning will be shown");
    logger.error("This error will be shown");

    console.println();
    console.println("=== Log with Time ===");
    console.println();

    Logger timeLogger(console);
    timeLogger.setShowTime(true);
    timeLogger.info("Message with timestamp");

    return 0;
}
