#include "termtools/components/table/table.h"
#include "termtools/core/internal/internal.h"
#include <algorithm>
#include <map>
#include <sstream>
#include <vector>

namespace termtools {

BorderStyle BorderStyle::get(BorderType type) {
    switch (type) {
    case BorderType::None:
        return {"", "", "", "", "", "", "", "", "", "", ""};

    case BorderType::ASCII:
        return {"+", "+", "+", "+", "-", "|", "+", "+", "+", "+", "+"};

    case BorderType::Unicode:
        return {"┌", "┐", "└", "┘", "─", "│", "├", "┤", "┬", "┴", "┼"};

    case BorderType::UnicodeRounded:
        return {"╭", "╮", "╰", "╯", "─", "│", "├", "┤", "┬", "┴", "┼"};

    case BorderType::UnicodeDouble:
        return {"╔", "╗", "╚", "╝", "═", "║", "╠", "╣", "╦", "╩", "╬"};

    case BorderType::Markdown:
        return {"|", "|", "|", "|", "-", "|", "|", "|", "|", "|", "|"};

    case BorderType::Dotted:
        return {"┌", "┐", "└", "┘", "╌", "│", "├", "┤", "┬", "┴", "┼"};

    case BorderType::Heavy:
        return {"┏", "┓", "┗", "┛", "━", "┃", "┣", "┫", "┳", "┻", "╋"};

    default:
        return {"┌", "┐", "└", "┘", "─", "│", "├", "┤", "┬", "┴", "┼"};
    }
}

Table::Table() {}

Table::Table(const std::vector<std::string> &headers) {
    for (const auto &header : headers) {
        addColumn(header);
    }
}

Table &Table::addColumn(const std::string &header) {
    Column col;
    col.header = header;
    columns_.push_back(col);
    return *this;
}

Table &Table::addColumn(const Column &column) {
    columns_.push_back(column);
    return *this;
}

Table &Table::addRow(std::initializer_list<std::string> cells) {
    std::vector<Cell> cellVec;
    for (const auto &cell : cells) {
        cellVec.push_back(Cell(cell));
    }
    rows_.push_back(cellVec);
    return *this;
}

Table &Table::addRow(const std::vector<std::string> &cells) {
    std::vector<Cell> cellVec;
    for (const auto &cell : cells) {
        cellVec.push_back(Cell(cell));
    }
    rows_.push_back(cellVec);
    return *this;
}

Table &Table::addRow(const std::vector<Text> &cells) {
    std::vector<Cell> cellVec;
    for (const auto &cell : cells) {
        cellVec.push_back(Cell(cell.render()));
    }
    rows_.push_back(cellVec);
    return *this;
}

Table &Table::addRow(const std::vector<Cell> &cells) {
    rows_.push_back(cells);
    return *this;
}

Table &Table::setHeaderStyle(const StyleBuilder &style) {
    headerStyle_ = style;
    return *this;
}

Table &Table::setHeaderColor(const Color &color) {
    headerStyle_.color(color);
    return *this;
}

Table &Table::setBorderStyle(const StyleBuilder &style) {
    borderStyle_ = style;
    return *this;
}

Table &Table::setRowStyle(const StyleBuilder &style) {
    rowStyle_ = style;
    return *this;
}

Table &Table::setBorderType(BorderType type) {
    borderType_ = type;
    showBorder_ = (type != BorderType::None);
    return *this;
}

Table &Table::setShowHeader(bool show) {
    showHeader_ = show;
    return *this;
}

Table &Table::setShowBorder(bool show) {
    showBorder_ = show;
    return *this;
}

Table &Table::setShowEdge(bool show) {
    showEdge_ = show;
    return *this;
}

Table &Table::setPadding(int padding) {
    padding_ = padding;
    return *this;
}

Table &Table::setTitle(const std::string &title) {
    title_ = title;
    return *this;
}

Table &Table::setCaption(const std::string &caption) {
    caption_ = caption;
    return *this;
}

Table &Table::setZebraStripes(bool enable) {
    zebraStripes_ = enable;
    return *this;
}

Table &Table::setZebraColors(const Color &odd, const Color &even) {
    zebraOddColor_ = odd;
    zebraEvenColor_ = even;
    zebraStripes_ = true;
    useZebraColors_ = true;
    return *this;
}

Table &Table::setZebraStyles(const StyleBuilder &odd,
                             const StyleBuilder &even) {
    zebraOddStyle_ = odd;
    zebraEvenStyle_ = even;
    zebraStripes_ = true;
    useZebraStyles_ = true;
    return *this;
}

Table &Table::setRowStyleCallback(RowStyleCallback callback) {
    rowStyleCallback_ = callback;
    return *this;
}

Table &Table::setCellStyleCallback(CellStyleCallback callback) {
    cellStyleCallback_ = callback;
    return *this;
}

Table &Table::highlightRow(int rowIndex, const StyleBuilder &style) {
    highlightedRows_[rowIndex] = style;
    return *this;
}

Table &Table::highlightColumn(int colIndex, const Color &color) {
    highlightedColumns_[colIndex] = color;
    return *this;
}

Table &Table::setCell(int row, int col, const std::string &content) {
    if (row >= 0 && row < static_cast<int>(rows_.size()) && col >= 0 &&
        col < static_cast<int>(columns_.size())) {
        rows_[row][col].content = content;
    }
    return *this;
}

Table &Table::setCellStyle(int row, int col, const CellStyle &style) {
    if (row >= 0 && row < static_cast<int>(rows_.size()) && col >= 0 &&
        col < static_cast<int>(columns_.size())) {
        rows_[row][col].style = style;
    }
    return *this;
}

Table &Table::setEmptyValue(const std::string &value) {
    emptyValue_ = value;
    return *this;
}

Table &Table::addFooterRow(const std::vector<std::string> &cells) {
    std::vector<Cell> cellVec;
    for (const auto &cell : cells) {
        cellVec.push_back(Cell(cell));
    }
    footerRows_.push_back(cellVec);
    return *this;
}

Table &Table::setFooterStyle(const StyleBuilder &style) {
    footerStyle_ = style;
    return *this;
}

BorderStyle Table::getBorderStyle() const {
    return BorderStyle::get(borderType_);
}

StyleBuilder Table::getEffectiveRowStyle(int rowIndex, int rowCount,
                                         bool isFooter) const {
    if (isFooter && !footerStyle_.toAnsi().empty()) {
        return footerStyle_;
    }

    if (highlightedRows_.count(rowIndex)) {
        return highlightedRows_.at(rowIndex);
    }

    if (rowStyleCallback_) {
        StyleBuilder customStyle = rowStyleCallback_(rowIndex, rowCount);
        if (!customStyle.toAnsi().empty()) {
            return customStyle;
        }
    }

    if (zebraStripes_ && !isFooter) {
        if (useZebraStyles_) {
            return (rowIndex % 2 == 0) ? zebraOddStyle_ : zebraEvenStyle_;
        }
    }

    return rowStyle_;
}

CellStyle Table::getEffectiveCellStyle(int row, int col,
                                       const std::string &content,
                                       bool isHeader, bool isFooter) const {
    CellStyle result;

    if (isHeader) {
        result.style = headerStyle_;
        if (!columns_[col].headerStyle.toAnsi().empty()) {
            result.style = columns_[col].headerStyle;
        }
        result.justify = columns_[col].justify;
        return result;
    }

    if (cellStyleCallback_) {
        CellStyle customStyle = cellStyleCallback_(row, col, content);
        if (customStyle.hasCustomStyle || customStyle.hasCustomJustify) {
            return customStyle;
        }
    }

    if (highlightedColumns_.count(col)) {
        result.color = highlightedColumns_.at(col);
        result.hasCustomStyle = true;
    }

    if (zebraStripes_ && !isFooter && useZebraColors_) {
        result.color = (row % 2 == 0) ? zebraOddColor_ : zebraEvenColor_;
        result.hasCustomStyle = true;
    }

    result.justify = columns_[col].justify;

    return result;
}

std::vector<int> Table::calculateColumnWidths() const {
    std::vector<int> widths(columns_.size(), 0);

    for (size_t i = 0; i < columns_.size(); ++i) {
        if (columns_[i].width > 0) {
            widths[i] = columns_[i].width;
        } else {
            widths[i] = internal::measureTextWidth(columns_[i].header);
        }

        if (columns_[i].minWidth > 0 && widths[i] < columns_[i].minWidth) {
            widths[i] = columns_[i].minWidth;
        }
    }

    for (const auto &row : rows_) {
        for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
            int cellWidth = internal::measureTextWidth(row[i].content);
            if (cellWidth > widths[i]) {
                widths[i] = cellWidth;
            }
        }
    }

    for (const auto &footerRow : footerRows_) {
        for (size_t i = 0; i < footerRow.size() && i < widths.size(); ++i) {
            int cellWidth = internal::measureTextWidth(footerRow[i].content);
            if (cellWidth > widths[i]) {
                widths[i] = cellWidth;
            }
        }
    }

    for (size_t i = 0; i < columns_.size(); ++i) {
        if (columns_[i].maxWidth > 0 && widths[i] > columns_[i].maxWidth) {
            widths[i] = columns_[i].maxWidth;
        }
    }

    return widths;
}

std::string Table::renderSeparator(const std::vector<int> &widths, bool top,
                                   bool bottom, bool beforeFooter) const {
    std::ostringstream oss;

    if (!showBorder_ || borderType_ == BorderType::None) {
        return "";
    }

    BorderStyle bs = getBorderStyle();

    oss << borderStyle_.toAnsi();

    if (borderType_ == BorderType::Markdown) {
        oss << bs.vertical;
    } else if (showEdge_) {
        if (top) {
            oss << bs.topLeft;
        } else if (bottom) {
            oss << bs.bottomLeft;
        } else {
            oss << bs.leftTee;
        }
    }

    for (size_t i = 0; i < widths.size(); ++i) {
        int totalWidth = widths[i] + padding_ * 2;
        for (int j = 0; j < totalWidth; ++j) {
            oss << bs.horizontal;
        }

        if (i < widths.size() - 1) {
            if (borderType_ == BorderType::Markdown) {
                oss << bs.vertical;
            } else if (top) {
                oss << bs.topTee;
            } else if (bottom) {
                oss << bs.bottomTee;
            } else {
                oss << bs.cross;
            }
        } else if (showEdge_) {
            if (borderType_ == BorderType::Markdown) {
                oss << bs.vertical;
            } else if (top) {
                oss << bs.topRight;
            } else if (bottom) {
                oss << bs.bottomRight;
            } else {
                oss << bs.rightTee;
            }
        }
    }

    oss << borderStyle_.toAnsiReset() << "\n";

    return oss.str();
}

std::string Table::renderTitleOrCaption(const std::string &text,
                                        int totalWidth) const {
    std::ostringstream oss;
    Text titleText(text);
    titleText.align(Justify::Center, totalWidth);
    oss << titleText.render() << "\n";
    return oss.str();
}

std::string Table::renderRow(const std::vector<Cell> &cells,
                             const std::vector<int> &widths, int rowIndex,
                             bool isHeader, bool isFooter) const {
    std::ostringstream oss;
    BorderStyle bs = getBorderStyle();

    if (showBorder_ && showEdge_ && borderType_ != BorderType::None) {
        oss << borderStyle_.toAnsi() << bs.vertical
            << borderStyle_.toAnsiReset();
    } else if (showBorder_ && borderType_ == BorderType::Markdown) {
        oss << borderStyle_.toAnsi() << bs.vertical
            << borderStyle_.toAnsiReset();
    }

    int rowCount = static_cast<int>(rows_.size());
    StyleBuilder effectiveRowStyle =
        getEffectiveRowStyle(rowIndex, rowCount, isFooter);

    for (size_t i = 0; i < widths.size(); ++i) {
        std::string padding(padding_, ' ');

        std::string content;
        if (i < cells.size()) {
            content = cells[i].content;
        }

        if (content.empty()) {
            content = emptyValue_;
        }

        int targetWidth = widths[i];
        int contentWidth = internal::measureTextWidth(content);
        if (contentWidth > targetWidth) {
            content = internal::truncateText(content, targetWidth);
        }

        CellStyle effectiveCellStyle =
            getEffectiveCellStyle(rowIndex, i, content, isHeader, isFooter);

        if (isHeader) {
            StyleBuilder style = headerStyle_;
            if (!columns_[i].headerStyle.toAnsi().empty()) {
                style = columns_[i].headerStyle;
            }

            oss << style.toAnsi() << padding;
            oss << internal::padText(content, widths[i], columns_[i].justify);
            oss << padding << style.toAnsiReset();
        } else {
            StyleBuilder cellStyle = effectiveRowStyle;

            if (effectiveCellStyle.hasCustomStyle) {
                cellStyle = effectiveCellStyle.style;
                if (!effectiveCellStyle.color.isDefault()) {
                    cellStyle.color(effectiveCellStyle.color);
                }
            }

            if (cells[i].style.hasCustomStyle) {
                cellStyle = cells[i].style.style;
            }

            oss << cellStyle.toAnsi() << padding;

            Justify justify = columns_[i].justify;
            if (effectiveCellStyle.hasCustomJustify) {
                justify = effectiveCellStyle.justify;
            }
            if (cells[i].style.hasCustomJustify) {
                justify = cells[i].style.justify;
            }

            oss << internal::padText(content, widths[i], justify);
            oss << padding << cellStyle.toAnsiReset();
        }

        if (showBorder_ && borderType_ != BorderType::None) {
            if (i < widths.size() - 1) {
                oss << borderStyle_.toAnsi() << bs.vertical
                    << borderStyle_.toAnsiReset();
            } else if (showEdge_ || borderType_ == BorderType::Markdown) {
                oss << borderStyle_.toAnsi() << bs.vertical
                    << borderStyle_.toAnsiReset();
            }
        }
    }

    oss << "\n";

    return oss.str();
}

std::string Table::render() const {
    std::ostringstream oss;

    auto widths = calculateColumnWidths();

    int totalWidth = 0;
    for (int w : widths) {
        totalWidth += w + padding_ * 2;
    }
    if (showBorder_ && borderType_ != BorderType::None) {
        totalWidth += static_cast<int>(widths.size() + 1);
    }

    if (!title_.empty()) {
        oss << renderTitleOrCaption(title_, totalWidth);
    }

    if (showBorder_ && borderType_ != BorderType::None) {
        oss << renderSeparator(widths, true, false);
    }

    if (showHeader_) {
        std::vector<Cell> headerCells;
        for (const auto &col : columns_) {
            headerCells.push_back(Cell(col.header));
        }
        oss << renderRow(headerCells, widths, -1, true, false);

        if (showBorder_ && borderType_ != BorderType::None) {
            oss << renderSeparator(widths, false, false);
        }
    }

    for (size_t i = 0; i < rows_.size(); ++i) {
        oss << renderRow(rows_[i], widths, static_cast<int>(i), false, false);
    }

    if (!footerRows_.empty() && showBorder_ &&
        borderType_ != BorderType::None) {
        oss << renderSeparator(widths, false, false, true);
    }

    for (size_t i = 0; i < footerRows_.size(); ++i) {
        oss << renderRow(footerRows_[i], widths, static_cast<int>(i), false,
                         true);
    }

    if (showBorder_ && borderType_ != BorderType::None) {
        oss << renderSeparator(widths, false, true);
    }

    if (!caption_.empty()) {
        oss << renderTitleOrCaption(caption_, totalWidth);
    }

    return oss.str();
}

void Table::print(Console &console) const { console.print(render()); }

int Table::getColumnCount() const { return static_cast<int>(columns_.size()); }

int Table::getRowCount() const { return static_cast<int>(rows_.size()); }

} // namespace termtools
