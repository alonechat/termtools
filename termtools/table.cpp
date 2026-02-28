#include "../termtools.h"
#include "internal.h"
#include <sstream>
#include <algorithm>

namespace termtools {

Table::Table() {}

Table::Table(const std::vector<std::string>& headers) {
    for (const auto& header : headers) {
        addColumn(header);
    }
}

Table& Table::addColumn(const std::string& header) {
    Column col;
    col.header = header;
    columns_.push_back(col);
    return *this;
}

Table& Table::addColumn(const Column& column) {
    columns_.push_back(column);
    return *this;
}

Table& Table::addRow(const std::vector<std::string>& cells) {
    std::vector<Text> textCells;
    for (const auto& cell : cells) {
        textCells.push_back(Text(cell));
    }
    rows_.push_back(textCells);
    return *this;
}

Table& Table::addRow(const std::vector<Text>& cells) {
    rows_.push_back(cells);
    return *this;
}

Table& Table::setHeaderStyle(const StyleBuilder& style) {
    headerStyle_ = style;
    return *this;
}

Table& Table::setHeaderColor(const Color& color) {
    headerStyle_.color(color);
    return *this;
}

Table& Table::setBorderStyle(const StyleBuilder& style) {
    borderStyle_ = style;
    return *this;
}

Table& Table::setRowStyle(const StyleBuilder& style) {
    rowStyle_ = style;
    return *this;
}

Table& Table::setShowHeader(bool show) {
    showHeader_ = show;
    return *this;
}

Table& Table::setShowBorder(bool show) {
    showBorder_ = show;
    return *this;
}

Table& Table::setShowEdge(bool show) {
    showEdge_ = show;
    return *this;
}

Table& Table::setPadding(int padding) {
    padding_ = padding;
    return *this;
}

Table& Table::setTitle(const std::string& title) {
    title_ = title;
    return *this;
}

Table& Table::setCaption(const std::string& caption) {
    caption_ = caption;
    return *this;
}

std::vector<int> Table::calculateColumnWidths() const {
    std::vector<int> widths(columns_.size(), 0);
    
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (columns_[i].width > 0) {
            widths[i] = columns_[i].width;
        } else {
            widths[i] = internal::measureTextWidth(columns_[i].header);
        }
    }
    
    for (const auto& row : rows_) {
        for (size_t i = 0; i < row.size() && i < widths.size(); ++i) {
            int cellWidth = row[i].width();
            if (cellWidth > widths[i]) {
                widths[i] = cellWidth;
            }
        }
    }
    
    return widths;
}

std::string Table::renderSeparator(const std::vector<int>& widths, bool top, bool bottom) const {
    std::ostringstream oss;
    
    if (!showBorder_) {
        return "";
    }
    
    oss << borderStyle_.toAnsi();
    
    if (showEdge_) {
        oss << (top ? "┌" : (bottom ? "└" : "├"));
    }
    
    for (size_t i = 0; i < widths.size(); ++i) {
        int totalWidth = widths[i] + padding_ * 2;
        for (int j = 0; j < totalWidth; ++j) {
            oss << "─";
        }
        
        if (i < widths.size() - 1) {
            oss << (top ? "┬" : (bottom ? "┴" : "┼"));
        } else if (showEdge_) {
            oss << (top ? "┐" : (bottom ? "┘" : "┤"));
        }
    }
    
    oss << borderStyle_.toAnsiReset() << "\n";
    
    return oss.str();
}

std::string Table::renderRow(const std::vector<Text>& cells, const std::vector<int>& widths, bool isHeader) const {
    std::ostringstream oss;
    
    if (showBorder_ && showEdge_) {
        oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset();
    }
    
    for (size_t i = 0; i < widths.size(); ++i) {
        std::string padding(padding_, ' ');
        
        if (isHeader) {
            StyleBuilder style = headerStyle_;
            if (!columns_[i].headerStyle.toAnsi().empty()) {
                style = columns_[i].headerStyle;
            }
            
            oss << style.toAnsi() << padding;
            
            if (i < cells.size()) {
                Text headerText = cells[i];
                headerText.align(columns_[i].justify, widths[i]);
                oss << headerText.render();
            } else {
                oss << internal::padText(columns_[i].header, widths[i], columns_[i].justify);
            }
            
            oss << padding << style.toAnsiReset();
        } else {
            oss << padding;
            
            if (i < cells.size()) {
                Text cellText = cells[i];
                cellText.align(columns_[i].justify, widths[i]);
                oss << cellText.render();
            } else {
                oss << std::string(widths[i], ' ');
            }
            
            oss << padding;
        }
        
        if (showBorder_) {
            if (i < widths.size() - 1) {
                oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset();
            } else if (showEdge_) {
                oss << borderStyle_.toAnsi() << "│" << borderStyle_.toAnsiReset();
            }
        }
    }
    
    oss << "\n";
    
    return oss.str();
}

std::string Table::render() const {
    std::ostringstream oss;
    
    if (!title_.empty()) {
        int totalWidth = 0;
        auto widths = calculateColumnWidths();
        for (int w : widths) {
            totalWidth += w + padding_ * 2;
        }
        if (showBorder_) {
            totalWidth += (widths.size() + 1);
        }
        
        Text titleText(title_);
        titleText.align(Justify::Center, totalWidth);
        oss << titleText.render() << "\n";
    }
    
    if (showBorder_) {
        oss << renderSeparator(calculateColumnWidths(), true, false);
    }
    
    if (showHeader_) {
        std::vector<Text> headerCells;
        for (const auto& col : columns_) {
            headerCells.push_back(Text(col.header));
        }
        oss << renderRow(headerCells, calculateColumnWidths(), true);
        
        if (showBorder_) {
            oss << renderSeparator(calculateColumnWidths(), false, false);
        }
    }
    
    for (const auto& row : rows_) {
        oss << renderRow(row, calculateColumnWidths(), false);
    }
    
    if (showBorder_) {
        oss << renderSeparator(calculateColumnWidths(), false, true);
    }
    
    if (!caption_.empty()) {
        int totalWidth = 0;
        auto widths = calculateColumnWidths();
        for (int w : widths) {
            totalWidth += w + padding_ * 2;
        }
        if (showBorder_) {
            totalWidth += (widths.size() + 1);
        }
        
        Text captionText(caption_);
        captionText.align(Justify::Center, totalWidth);
        oss << captionText.render() << "\n";
    }
    
    return oss.str();
}

void Table::print(Console& console) const {
    console.print(render());
}

int Table::getColumnCount() const {
    return static_cast<int>(columns_.size());
}

int Table::getRowCount() const {
    return static_cast<int>(rows_.size());
}

}
