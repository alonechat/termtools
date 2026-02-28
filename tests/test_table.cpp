#include "termtools.h"
#include <cassert>
#include <iostream>

using namespace termtools;

void test_table_creation() {
    Table table;
    assert(table.getColumnCount() == 0);
    assert(table.getRowCount() == 0);
    std::cout << "✓ Table creation test passed" << std::endl;
}

void test_table_columns() {
    Table table;
    table.addColumn("Name").addColumn("Age");
    
    assert(table.getColumnCount() == 2);
    std::cout << "✓ Table columns test passed" << std::endl;
}

void test_table_rows() {
    Table table;
    table.addColumn("Name").addColumn("Age");
    table.addRow({"Alice", "28"});
    table.addRow({"Bob", "35"});
    
    assert(table.getRowCount() == 2);
    std::cout << "✓ Table rows test passed" << std::endl;
}

void test_table_render() {
    Table table;
    table.addColumn("A").addColumn("B");
    table.addRow({"1", "2"});
    
    std::string rendered = table.render();
    assert(!rendered.empty());
    assert(rendered.find("A") != std::string::npos);
    assert(rendered.find("B") != std::string::npos);
    std::cout << "✓ Table render test passed" << std::endl;
}

int main() {
    std::cout << "Running Table tests..." << std::endl;
    std::cout << std::endl;
    
    test_table_creation();
    test_table_columns();
    test_table_rows();
    test_table_render();
    
    std::cout << std::endl;
    std::cout << "All Table tests passed!" << std::endl;
    
    return 0;
}
