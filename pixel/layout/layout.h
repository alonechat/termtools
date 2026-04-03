#pragma once
#include "../buffer.h"
#include "../primitives/primitive.h"
#include <vector>

namespace tt {
namespace pixel {

class Layout {
public:
    virtual ~Layout() = default;
    virtual void layout(const std::vector<PrimitivePtr>& children, const Rect& bounds) = 0;
    virtual Rect query(PrimitivePtr child) const = 0;

protected:
    std::vector<Rect> child_bounds_;
};

}
}