#pragma once
#include "../buffer.h"
#include <memory>

namespace tt {
namespace pixel {

class Effect {
public:
    virtual ~Effect() = default;
    virtual void apply(Pixel& pixel) const = 0;
    virtual Effect* clone() const = 0;
};

using EffectPtr = std::shared_ptr<Effect>;

}
}