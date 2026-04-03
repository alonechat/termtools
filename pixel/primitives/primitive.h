#pragma once
#include "../buffer.h"
#include "../canvas.h"
#include <vector>
#include <memory>

namespace tt {
namespace pixel {

class Primitive {
public:
    virtual ~Primitive() = default;
    virtual void draw(Canvas& canvas) = 0;
    virtual Rect bounds() const = 0;
    virtual void setZIndex(int16_t z) { z_index_ = z; }
    virtual int16_t zIndex() const { return z_index_; }

protected:
    int16_t z_index_ = 0;
};

using PrimitivePtr = std::shared_ptr<Primitive>;

}
}