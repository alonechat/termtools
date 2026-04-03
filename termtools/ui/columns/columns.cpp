#include "termtools/ui/columns/columns.h"
#include "termtools/core/internal/internal.h"
#include <vector>
#include <sstream>
#include <algorithm>

namespace termtools {

Columns::Columns()
    : expand_(false)
    , padding_(1)
    , equalWidth_(false) {}

Columns::Columns(int columnCount)
    : expand_(false)
    , padding_(1)
    , equalWidth_(false) {
    columns_.resize(columnCount);
}

Columns& Columns::add(const Text& text) {
    if (columns_.empty()) {
        columns_.push_back({text});
    } else {
        columns_[0].push_back(text);
    }
    return *this;
}

Columns& Columns::add(const std::string& text) {
    return add(Text(text));
}

Columns& Columns::add(const std::vector<Text>& texts) {
    for (const auto& text : texts) {
        add(text);
    }
    return *this;
}

Columns& Columns::setExpand(bool expand) {
    expand_ = expand;
    return *this;
}

Columns& Columns::setPadding(int padding) {
    padding_ = padding;
    return *this;
}

Columns& Columns::setEqualWidth(bool equalWidth) {
    equalWidth_ = equalWidth;
    return *this;
}

std::vector<int> Columns::calculateColumnWidths() const {
    std::vector<int> widths;
    
    for (const auto& column : columns_) {
        int maxWidth = 0;
        for (const auto& text : column) {
            maxWidth = std::max(maxWidth, text.width());
        }
        widths.push_back(maxWidth);
    }
    
    if (equalWidth_ && !widths.empty()) {
        int maxWidth = *std::max_element(widths.begin(), widths.end());
        std::fill(widths.begin(), widths.end(), maxWidth);
    }
    
    return widths;
}

std::string Columns::render() const {
    std::ostringstream oss;
    
    if (columns_.empty()) {
        return "";
    }
    
    auto widths = calculateColumnWidths();
    
    size_t maxRows = 0;
    for (const auto& column : columns_) {
        maxRows = std::max(maxRows, column.size());
    }
    
    for (size_t row = 0; row < maxRows; ++row) {
        for (size_t col = 0; col < columns_.size(); ++col) {
            if (col > 0) {
                oss << std::string(padding_, ' ');
            }
            
            int width = widths[col];
            
            if (row < columns_[col].size()) {
                Text text = columns_[col][row];
                text.align(Justify::Left, width);
                oss << text.render();
            } else {
                oss << std::string(width, ' ');
            }
        }
        oss << "\n";
    }
    
    return oss.str();
}

void Columns::print(Console& console) const {
    console.print(render());
}

}
