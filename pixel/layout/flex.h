#pragma once
#include "layout.h"

namespace tt {
namespace pixel {

enum class FlexDirection {
    ROW,
    COLUMN,
};

enum class JustifyContent {
    FLEX_START,
    CENTER,
    FLEX_END,
    SPACE_BETWEEN,
    SPACE_AROUND,
};

enum class AlignItems {
    FLEX_START,
    CENTER,
    FLEX_END,
    STRETCH,
};

class FlexLayout : public Layout {
public:
    FlexLayout(FlexDirection dir = FlexDirection::ROW, JustifyContent justify = JustifyContent::FLEX_START, AlignItems align = AlignItems::CENTER)
        : direction_(dir), justify_(justify), align_(align), gap_(0) {}

    void layout(const std::vector<PrimitivePtr>& children, const Rect& bounds) override {
        bounds_ = bounds;
        children_ = children;
        child_bounds_.resize(children.size());

        if (children.empty()) return;

        bool isRow = direction_ == FlexDirection::ROW;
        size_t childCount = children.size();
        uint16_t totalGap = gap_ * static_cast<uint16_t>(childCount > 0 ? childCount - 1 : 0);

        if (isRow) {
            uint16_t totalWidth = bounds.width;
            uint16_t totalHeight = bounds.height;
            int16_t childWidth = static_cast<int16_t>((totalWidth - totalGap) / childCount);
            int16_t childHeight = static_cast<int16_t>(totalHeight);

            int16_t offsetX = bounds.x;

            if (justify_ == JustifyContent::SPACE_BETWEEN || justify_ == JustifyContent::SPACE_AROUND) {
                int16_t extraSpace = totalWidth - (childWidth * static_cast<int16_t>(childCount)) - totalGap;
                if (justify_ == JustifyContent::SPACE_AROUND) {
                    offsetX = bounds.x + extraSpace / 2;
                } else if (justify_ == JustifyContent::SPACE_BETWEEN) {
                    if (childCount > 1) {
                        childWidth = static_cast<int16_t>((totalWidth - totalGap) / childCount);
                    }
                }
            } else if (justify_ == JustifyContent::CENTER) {
                offsetX = bounds.x + (totalWidth - childWidth * static_cast<int16_t>(childCount) - totalGap) / 2;
            } else if (justify_ == JustifyContent::FLEX_END) {
                offsetX = bounds.x + totalWidth - childWidth * static_cast<int16_t>(childCount) - totalGap;
            }

            for (size_t i = 0; i < childCount; ++i) {
                int16_t x = offsetX;
                int16_t y = bounds.y;
                int16_t w = childWidth;
                int16_t h = childHeight;

                if (align_ == AlignItems::CENTER) {
                    y = bounds.y + (totalHeight - childHeight) / 2;
                } else if (align_ == AlignItems::FLEX_END) {
                    y = bounds.y + totalHeight - childHeight;
                } else if (align_ == AlignItems::STRETCH) {
                    h = totalHeight;
                }

                child_bounds_[i] = {static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(w), static_cast<uint16_t>(h)};
                offsetX += childWidth + gap_;
            }
        } else {
            uint16_t totalWidth = bounds.width;
            uint16_t totalHeight = bounds.height;
            int16_t childWidth = static_cast<int16_t>(totalWidth);
            int16_t childHeight = static_cast<int16_t>((totalHeight - totalGap) / childCount);

            int16_t offsetY = bounds.y;

            if (justify_ == JustifyContent::SPACE_BETWEEN || justify_ == JustifyContent::SPACE_AROUND) {
                int16_t extraSpace = totalHeight - (childHeight * static_cast<int16_t>(childCount)) - totalGap;
                if (justify_ == JustifyContent::SPACE_AROUND) {
                    offsetY = bounds.y + extraSpace / 2;
                } else if (justify_ == JustifyContent::SPACE_BETWEEN) {
                    if (childCount > 1) {
                        childHeight = static_cast<int16_t>((totalHeight - totalGap) / childCount);
                    }
                }
            } else if (justify_ == JustifyContent::CENTER) {
                offsetY = bounds.y + (totalHeight - childHeight * static_cast<int16_t>(childCount) - totalGap) / 2;
            } else if (justify_ == JustifyContent::FLEX_END) {
                offsetY = bounds.y + totalHeight - childHeight * static_cast<int16_t>(childCount) - totalGap;
            }

            for (size_t i = 0; i < childCount; ++i) {
                int16_t x = bounds.x;
                int16_t y = offsetY;
                int16_t w = childWidth;
                int16_t h = childHeight;

                if (align_ == AlignItems::CENTER) {
                    x = bounds.x + (totalWidth - childWidth) / 2;
                } else if (align_ == AlignItems::FLEX_END) {
                    x = bounds.x + totalWidth - childWidth;
                } else if (align_ == AlignItems::STRETCH) {
                    w = totalWidth;
                }

                child_bounds_[i] = {static_cast<uint16_t>(x), static_cast<uint16_t>(y), static_cast<uint16_t>(w), static_cast<uint16_t>(h)};
                offsetY += childHeight + gap_;
            }
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

    void setDirection(FlexDirection dir) { direction_ = dir; }
    void setJustify(JustifyContent j) { justify_ = j; }
    void setAlign(AlignItems a) { align_ = a; }
    void setGap(uint16_t g) { gap_ = g; }

    FlexDirection direction() const { return direction_; }
    JustifyContent justify() const { return justify_; }
    AlignItems align() const { return align_; }
    uint16_t gap() const { return gap_; }

private:
    FlexDirection direction_;
    JustifyContent justify_;
    AlignItems align_;
    uint16_t gap_;

    std::vector<PrimitivePtr> children_;
    Rect bounds_;
};

}
}