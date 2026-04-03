#pragma once
#include "layout.h"

namespace tt {
namespace pixel {

class GridLayout : public Layout {
public:
    GridLayout(uint16_t rows = 1, uint16_t cols = 1)
        : rows_(rows), cols_(cols), cell_padding_(0) {}

    void layout(const std::vector<PrimitivePtr>& children, const Rect& bounds) override {
        bounds_ = bounds;
        children_ = children;
        child_bounds_.resize(children.size());

        if (children.empty() || rows_ == 0 || cols_ == 0) return;

        for (size_t i = 0; i < children.size(); ++i) {
            uint16_t row = i / cols_;
            uint16_t col = i % cols_;

            if (row >= rows_) {
                child_bounds_[i] = {0, 0, 0, 0};
                continue;
            }

            child_bounds_[i] = getCellRect(row, col);
        }
    }

    Rect query(PrimitivePtr child) const override {
        for (size_t i = 0; i < children_.size(); ++i) {
            if (children_[i].get() == child.get()) {
                return child_bounds_[i];
            }
        }
        return {0, 0, 0, 0};
    }

    void setGrid(uint16_t rows, uint16_t cols) { rows_ = rows; cols_ = cols; }
    void setCellPadding(uint16_t p) { cell_padding_ = p; }

    uint16_t rows() const { return rows_; }
    uint16_t cols() const { return cols_; }
    uint16_t cellPadding() const { return cell_padding_; }

private:
    uint16_t rows_, cols_;
    uint16_t cell_padding_;

    std::vector<PrimitivePtr> children_;
    Rect bounds_;

    Rect getCellRect(uint16_t row, uint16_t col) const {
        uint16_t cellWidth = bounds_.width / cols_;
        uint16_t cellHeight = bounds_.height / rows_;

        uint16_t x = bounds_.x + col * cellWidth + cell_padding_;
        uint16_t y = bounds_.y + row * cellHeight + cell_padding_;
        uint16_t w = cellWidth - cell_padding_ * 2;
        uint16_t h = cellHeight - cell_padding_ * 2;

        if (w < cell_padding_ * 2) w = 0;
        if (h < cell_padding_ * 2) h = 0;

        return {x, y, w, h};
    }
};

}
}