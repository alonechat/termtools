#include "termtools.h"

using namespace termtools;

int main() {
    Console console;
    
    console.println("=== Basic Table ===");
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
    console.println("=== Styled Table ===");
    console.println();
    
    Table table2;
    table2.addColumn("Product");
    table2.addColumn("Price");
    table2.addColumn("Stock");
    table2.addRow({"Laptop", "$999", "50"});
    table2.addRow({"Mouse", "$29", "200"});
    table2.addRow({"Keyboard", "$79", "150"});
    table2.addRow({"Monitor", "$399", "75"});
    table2.setHeaderStyle(StyleBuilder().bold().color(Color(ColorName::Yellow)));
    table2.setBorderStyle(StyleBuilder().color(Color(ColorName::Cyan)));
    table2.setTitle("Product Inventory");
    table2.setCaption("Updated: 2024-01-15");
    table2.print(console);
    
    console.println();
    console.println("=== Table with Alignment ===");
    console.println();
    
    Table table3;
    Table::Column col1;
    col1.header = "Left";
    col1.justify = Justify::Left;
    
    Table::Column col2;
    col2.header = "Center";
    col2.justify = Justify::Center;
    
    Table::Column col3;
    col3.header = "Right";
    col3.justify = Justify::Right;
    
    table3.addColumn(col1);
    table3.addColumn(col2);
    table3.addColumn(col3);
    table3.addRow({"Left", "Center", "Right"});
    table3.addRow({"A", "B", "C"});
    table3.addRow({"123", "456", "789"});
    table3.print(console);
    
    console.println();
    console.println("=== No Border Table ===");
    console.println();
    
    Table table4;
    table4.addColumn("Key");
    table4.addColumn("Value");
    table4.addRow({"Name", "TermTools"});
    table4.addRow({"Version", "1.0.0"});
    table4.addRow({"Language", "C++14"});
    table4.addRow({"License", "MIT"});
    table4.setShowBorder(false);
    table4.print(console);
    
    return 0;
}
