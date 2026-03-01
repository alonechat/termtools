#ifndef TERMTOOLS_TABLE_H
#define TERMTOOLS_TABLE_H

#include "termtools/core/console.h"
#include "termtools/core/style.h"
#include "termtools/text/text.h"
#include "termtools/utils/utils.h"

#include <string>
#include <vector>

namespace termtools {

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

} 

#endif
