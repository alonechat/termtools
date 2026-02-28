#include "termtools.h"
#include <thread>
#include <chrono>

using namespace termtools;

int main() {
    Console console;
    
    console.println();
    Text title("TermTools - Modern C++ Terminal Library");
    title.styled(StyleBuilder().bold().color(Color(ColorName::Cyan)));
    title.align(Justify::Center, console.getTerminalWidth());
    console.println(title);
    console.println();
    
    Text section1("1. Text Formatting");
    section1.styled(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    console.println(section1);
    
    Text colored("This is red text");
    colored.colored(Color(ColorName::Red));
    console.println(colored);
    
    Text styled("This is bold and underlined text");
    styled.styled(StyleBuilder().bold().underline());
    console.println(styled);
    
    Text rgb("This is RGB colored text");
    rgb.colored(Color(255, 100, 50));
    console.println(rgb);
    
    console.println();
    
    Text section2("2. Table Example");
    section2.styled(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    console.println(section2);
    
    Table table;
    table.addColumn("ID").addColumn("Name").addColumn("Status").addColumn("Progress");
    table.addRow({"1", "Task Alpha", "Running", "75%"});
    table.addRow({"2", "Task Beta", "Completed", "100%"});
    table.addRow({"3", "Task Gamma", "Pending", "0%"});
    table.addRow({"4", "Task Delta", "Failed", "45%"});
    table.setHeaderColor(Color(ColorName::Cyan));
    table.print(console);
    
    console.println();
    
    Text section3("3. Progress Bar Example");
    section3.styled(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    console.println(section3);
    
    ProgressBar progress(100, "Processing items");
    progress.start();
    
    for (int i = 0; i <= 100; ++i) {
        progress.update(1);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
    
    console.println();
    
    Text section4("4. Panel Example");
    section4.styled(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    console.println(section4);
    
    Panel panel("Information Panel", "This is a panel with some content.\nIt can span multiple lines.\nAnd supports Unicode: 你好世界 🎉");
    panel.setTitleColor(Color(ColorName::Green));
    panel.print(console);
    
    console.println();
    
    Text section5("5. Logger Example");
    section5.styled(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    console.println(section5);
    
    Logger logger;
    logger.debug("This is a debug message");
    logger.info("This is an info message");
    logger.warning("This is a warning message");
    logger.error("This is an error message");
    logger.critical("This is a critical message");
    
    console.println();
    
    Text section6("6. Markup Example");
    section6.styled(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    console.println(section6);
    
    Text markup = Text::fromMarkup("This is [bold]bold[/bold], [red]red[/red], and [underline]underlined[/underline] text!");
    console.println(markup);
    
    console.println();
    
    Text footer("All features demonstrated successfully!");
    footer.styled(StyleBuilder().bold().color(Color(ColorName::Green)));
    footer.align(Justify::Center, console.getTerminalWidth());
    console.println(footer);
    
    return 0;
}
