#include "termtools.h"

using namespace termtools;

int main() {
    Console console;

    console.println("=== 1. Basic Table ===");
    console.println();

    Table table1;
    table1.addColumn("Name");
    table1.addColumn("Age");
    table1.addColumn("City");
    table1.addRow({"Alice", "28", "New York"});
    table1.addRow({"Bob", "35", "Los Angeles"});
    table1.addRow({"Charlie", "42", "Chicago"});
    table1.print(console);

    console.println();
    console.println("=== 2. Border Types Demo ===");
    console.println();

    console.println("Unicode (Default):");
    Table t1;
    t1.addColumn("A").addColumn("B").addColumn("C");
    t1.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t1.setBorderType(BorderType::Unicode);
    t1.print(console);

    console.println();
    console.println("ASCII:");
    Table t2;
    t2.addColumn("A").addColumn("B").addColumn("C");
    t2.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t2.setBorderType(BorderType::ASCII);
    t2.print(console);

    console.println();
    console.println("Unicode Rounded:");
    Table t3;
    t3.addColumn("A").addColumn("B").addColumn("C");
    t3.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t3.setBorderType(BorderType::UnicodeRounded);
    t3.print(console);

    console.println();
    console.println("Unicode Double:");
    Table t4;
    t4.addColumn("A").addColumn("B").addColumn("C");
    t4.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t4.setBorderType(BorderType::UnicodeDouble);
    t4.print(console);

    console.println();
    console.println("Heavy:");
    Table t5;
    t5.addColumn("A").addColumn("B").addColumn("C");
    t5.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t5.setBorderType(BorderType::Heavy);
    t5.print(console);

    console.println();
    console.println("Markdown:");
    Table t6;
    t6.addColumn("A").addColumn("B").addColumn("C");
    t6.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t6.setBorderType(BorderType::Markdown);
    t6.print(console);

    console.println();
    console.println("Dotted:");
    Table t7;
    t7.addColumn("A").addColumn("B").addColumn("C");
    t7.addRow({"1", "2", "3"}).addRow({"4", "5", "6"});
    t7.setBorderType(BorderType::Dotted);
    t7.print(console);

    console.println();
    console.println("=== 3. Zebra Stripes ===");
    console.println();

    Table zebraTable;
    zebraTable.addColumn("ID").addColumn("Name").addColumn("Score");
    zebraTable.addRow({"1", "Alice", "95"});
    zebraTable.addRow({"2", "Bob", "87"});
    zebraTable.addRow({"3", "Charlie", "92"});
    zebraTable.addRow({"4", "Diana", "88"});
    zebraTable.addRow({"5", "Eve", "91"});
    zebraTable.setZebraColors(Color(ColorName::Black), Color(ColorName::White));
    zebraTable.setBorderStyle(StyleBuilder().color(Color(ColorName::Cyan)));
    zebraTable.print(console);

    console.println();
    console.println("=== 4. Row Highlighting ===");
    console.println();

    Table highlightTable;
    highlightTable.addColumn("Status").addColumn("Service").addColumn("Port");
    highlightTable.addRow({"Running", "HTTP Server", "8080"});
    highlightTable.addRow({"Stopped", "Database", "5432"});
    highlightTable.addRow({"Running", "Cache", "6379"});
    highlightTable.addRow({"Error", "Queue", "5672"});
    highlightTable.addRow({"Running", "API Gateway", "3000"});

    highlightTable.highlightRow(
        1, StyleBuilder().color(Color(ColorName::Red)).bold());
    highlightTable.highlightRow(
        3, StyleBuilder().color(Color(ColorName::Yellow)).bold());
    highlightTable.setBorderStyle(StyleBuilder().color(Color(ColorName::Blue)));
    highlightTable.print(console);

    console.println();
    console.println("=== 5. Column Highlighting ===");
    console.println();

    Table colHighlight;
    colHighlight.addColumn("Product").addColumn("Price").addColumn("Change");
    colHighlight.addRow({"Apple", "$1.50", "+5%"});
    colHighlight.addRow({"Banana", "$0.80", "-2%"});
    colHighlight.addRow({"Orange", "$1.20", "+3%"});
    colHighlight.highlightColumn(1, Color(ColorName::Green));
    colHighlight.highlightColumn(2, Color(ColorName::Yellow));
    colHighlight.print(console);

    console.println();
    console.println("=== 6. Cell-Level Styling ===");
    console.println();

    Table cellStyleTable;
    cellStyleTable.addColumn("Name").addColumn("Status").addColumn("Progress");

    Table::Cell cell1("Task A");
    Table::Cell status1("Done");
    status1.style.style = StyleBuilder().color(Color(ColorName::Green)).bold();
    status1.style.hasCustomStyle = true;
    cellStyleTable.addRow({cell1, status1, Table::Cell("100%")});

    Table::Cell cell2("Task B");
    Table::Cell status2("In Progress");
    status2.style.style = StyleBuilder().color(Color(ColorName::Yellow));
    status2.style.hasCustomStyle = true;
    cellStyleTable.addRow({cell2, status2, Table::Cell("60%")});

    Table::Cell cell3("Task C");
    Table::Cell status3("Failed");
    status3.style.style = StyleBuilder().color(Color(ColorName::Red)).bold();
    status3.style.hasCustomStyle = true;
    cellStyleTable.addRow({cell3, status3, Table::Cell("0%")});

    cellStyleTable.print(console);

    console.println();
    console.println("=== 7. Footer Row (Summary) ===");
    console.println();

    Table summaryTable;
    summaryTable.addColumn("Item").addColumn("Quantity").addColumn("Price");
    summaryTable.addRow({"Laptop", "2", "$1998"});
    summaryTable.addRow({"Mouse", "5", "$145"});
    summaryTable.addRow({"Keyboard", "3", "$237"});
    summaryTable.addFooterRow({"Total", "10", "$2380"});
    summaryTable.setFooterStyle(
        StyleBuilder().bold().color(Color(ColorName::Cyan)));
    summaryTable.setTitle("Order Summary");
    summaryTable.setCaption("Thank you for your order!");
    summaryTable.print(console);

    console.println();
    console.println("=== 8. Min/Max Column Width ===");
    console.println();

    Table widthTable;
    Table::Column col1;
    col1.header = "ID";
    col1.minWidth = 5;
    col1.maxWidth = 5;

    Table::Column col2;
    col2.header = "Description";
    col2.minWidth = 20;
    col2.maxWidth = 20;

    Table::Column col3;
    col3.header = "Status";
    col3.minWidth = 10;

    widthTable.addColumn(col1).addColumn(col2).addColumn(col3);
    widthTable.addRow(
        {"001", "This is a very long description that should be truncated",
         "Active"});
    widthTable.addRow({"002", "Short desc", "Inactive"});
    widthTable.print(console);

    console.println();
    console.println("=== 9. Empty Value Handling ===");
    console.println();

    Table emptyTable;
    emptyTable.addColumn("Name").addColumn("Email").addColumn("Phone");
    emptyTable.addRow({"Alice", "alice@example.com", "555-1234"});
    emptyTable.addRow({"Bob", "", "555-5678"});
    emptyTable.addRow({"Charlie", "charlie@example.com", ""});
    emptyTable.addRow({"Diana", "", ""});
    emptyTable.setEmptyValue("N/A");
    emptyTable.setBorderStyle(StyleBuilder().color(Color(ColorName::Magenta)));
    emptyTable.print(console);

    console.println();
    console.println("=== 10. Callback-Based Styling ===");
    console.println();

    Table callbackTable;
    callbackTable.addColumn("Value").addColumn("Status");
    callbackTable.addRow({"100", "OK"});
    callbackTable.addRow({"-50", "Warning"});
    callbackTable.addRow({"200", "OK"});
    callbackTable.addRow({"-10", "Warning"});

    callbackTable.setCellStyleCallback(
        [](int row, int col, const std::string &content) -> CellStyle {
            CellStyle style;
            if (col == 0 && content[0] == '-') {
                style.style = StyleBuilder().color(Color(ColorName::Red));
                style.hasCustomStyle = true;
            }
            if (col == 1 && content == "Warning") {
                style.style =
                    StyleBuilder().color(Color(ColorName::Yellow)).bold();
                style.hasCustomStyle = true;
            }
            return style;
        });

    callbackTable.print(console);

    console.println();
    console.println("=== 11. Complete Styled Table ===");
    console.println();

    Table completeTable;
    completeTable.addColumn("Product");
    completeTable.addColumn("Price");
    completeTable.addColumn("Stock");
    completeTable.addColumn("Status");
    completeTable.addRow({"Laptop", "$999", "50", "Available"});
    completeTable.addRow({"Mouse", "$29", "200", "Available"});
    completeTable.addRow({"Keyboard", "$79", "0", "Out of Stock"});
    completeTable.addRow({"Monitor", "$399", "75", "Available"});
    completeTable.addFooterRow({"Subtotal", "$1506", "325", "-"});

    completeTable.setTitle("Product Inventory");
    completeTable.setCaption("Updated: 2026-01-15");
    completeTable.setHeaderStyle(
        StyleBuilder().bold().color(Color(ColorName::Yellow)));
    completeTable.setBorderStyle(StyleBuilder().color(Color(ColorName::Cyan)));
    completeTable.setBorderType(BorderType::UnicodeRounded);
    completeTable.setFooterStyle(
        StyleBuilder().bold().color(Color(ColorName::Green)));
    completeTable.setZebraStripes(true);
    completeTable.highlightRow(
        2, StyleBuilder().color(Color(ColorName::Red)).dim());
    completeTable.setEmptyValue("-");

    completeTable.print(console);

    return 0;
}
