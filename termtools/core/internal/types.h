#ifndef TERMTOOLS_TYPES_H
#define TERMTOOLS_TYPES_H

#include <cstdint>

namespace termtools {

enum class Justify : uint8_t { Left, Center, Right };
enum class Overflow : uint8_t { Fold, Crop, Ellipsis, Ignore };

}

#endif
