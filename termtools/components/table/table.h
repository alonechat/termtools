#ifndef TERMTOOLS_TABLE_H
#define TERMTOOLS_TABLE_H

#include "termtools/core/output/console.h"
#include "termtools/core/formatting/style.h"
#include "termtools/core/formatting/text.h"
#include "termtools/utils/utils.h"

#include <string>
#include <vector>
#include <functional>
#include <variant>
#include <map>
#include <initializer_list>

namespace termtools {

enum class BorderType {
    None,
    ASCII,
    Unicode,
    UnicodeRounded,
    UnicodeDouble,
    Markdown,
    Dotted,
    Heavy
};

struct BorderStyle {
    std::string topLeft;
    std::string topRight;
    std::string bottomLeft;
    std::string bottomRight;
    std::string horizontal;
    std::string vertical;
    std::string leftTee;
    std::string rightTee;
    std::string topTee;
    std::string bottomTee;
    std::string cross;
    
    static BorderStyle get(BorderType type);
};

struct CellStyle {
    Color color;
    StyleBuilder style;
    Justify justify = Justify::Left;
    bool hasCustomStyle = false;
    bool hasCustomJustify = false;
};

class Table {
  public:
    struct Column {
        std::string header;
        Justify justify = Justify::Left;
        int width = 0;
        int minWidth = 0;
        int maxWidth = 0;
        bool no_wrap = false;
        Color headerColor;
        StyleBuilder headerStyle;
        bool visible = true;
    };

    struct Cell {
        std::string content;
        CellStyle style;
        
        Cell() = default;
        Cell(const std::string& c) : content(c) {}
        Cell(const char* c) : content(c) {}
        Cell(const std::string& c, const CellStyle& s) : content(c), style(s) {}
    };

    using RowStyleCallback = std::function<StyleBuilder(int rowIndex, int rowCount)>;
    using CellStyleCallback = std::function<CellStyle(int row, int col, const std::string& content)>;

    Table();
    explicit Table(const std::vector<std::string> &headers);

    Table &addColumn(const std::string &header);
    Table &addColumn(const Column &column);

    Table &addRow(std::initializer_list<std::string> cells);
    Table &addRow(const std::vector<std::string> &cells);
    Table &addRow(const std::vector<Text> &cells);
    Table &addRow(const std::vector<Cell> &cells);

    Table &setHeaderStyle(const StyleBuilder &style);
    Table &setHeaderColor(const Color &color);
    Table &setBorderStyle(const StyleBuilder &style);
    Table &setRowStyle(const StyleBuilder &style);
    Table &setBorderType(BorderType type);

    Table &setShowHeader(bool show);
    Table &setShowBorder(bool show);
    Table &setShowEdge(bool show);
    Table &setPadding(int padding);

    Table &setTitle(const std::string &title);
    Table &setCaption(const std::string &caption);

    Table &setZebraStripes(bool enable);
    Table &setZebraColors(const Color &odd, const Color &even);
    Table &setZebraStyles(const StyleBuilder &odd, const StyleBuilder &even);

    Table &setRowStyleCallback(RowStyleCallback callback);
    Table &setCellStyleCallback(CellStyleCallback callback);

    Table &highlightRow(int rowIndex, const StyleBuilder &style);
    Table &highlightColumn(int colIndex, const Color &color);
    Table &setCell(int row, int col, const std::string &content);
    Table &setCellStyle(int row, int col, const CellStyle &style);

    Table &setEmptyValue(const std::string &value);

    Table &addFooterRow(const std::vector<std::string> &cells);
    Table &setFooterStyle(const StyleBuilder &style);

    std::string render() const;
    void print(Console &console) const;

    int getColumnCount() const;
    int getRowCount() const;

  private:
    std::vector<Column> columns_;
    std::vector<std::vector<Cell>> rows_;
    std::vector<std::vector<Cell>> footerRows_;
    StyleBuilder headerStyle_;
    StyleBuilder borderStyle_;
    StyleBuilder rowStyle_;
    StyleBuilder footerStyle_;
    BorderType borderType_ = BorderType::Unicode;
    bool showHeader_ = true;
    bool showBorder_ = true;
    bool showEdge_ = true;
    int padding_ = 1;
    std::string title_;
    std::string caption_;
    std::string emptyValue_ = "";
    
    bool zebraStripes_ = false;
    Color zebraOddColor_;
    Color zebraEvenColor_;
    StyleBuilder zebraOddStyle_;
    StyleBuilder zebraEvenStyle_;
    bool useZebraColors_ = false;
    bool useZebraStyles_ = false;
    
    RowStyleCallback rowStyleCallback_;
    CellStyleCallback cellStyleCallback_;
    
    std::map<int, StyleBuilder> highlightedRows_;
    std::map<int, Color> highlightedColumns_;

    std::vector<int> calculateColumnWidths() const;
    std::string renderRow(const std::vector<Cell> &cells,
                          const std::vector<int> &widths,
                          int rowIndex = -1,
                          bool isHeader = false,
                          bool isFooter = false) const;
    std::string renderSeparator(const std::vector<int> &widths,
                                bool top = false, 
                                bool bottom = false,
                                bool beforeFooter = false) const;
    std::string renderTitleOrCaption(const std::string &text, int totalWidth) const;
    BorderStyle getBorderStyle() const;
    StyleBuilder getEffectiveRowStyle(int rowIndex, int rowCount, bool isFooter = false) const;
    CellStyle getEffectiveCellStyle(int row, int col, const std::string& content, bool isHeader = false, bool isFooter = false) const;
};

}
#endif
