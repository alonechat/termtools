#pragma once
#include "../buffer.h"
#include <memory>

namespace tt {
namespace pixel {

class Backend {
public:
    virtual ~Backend() = default;
    
    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void clear() = 0;
    virtual void flush() = 0;
    virtual void setPixel(uint16_t x, uint16_t y, const Pixel& pixel) = 0;
    virtual void moveCursor(uint16_t x, uint16_t y) = 0;
    virtual void beginBatch() = 0;
    virtual void endBatch() = 0;
    
    virtual uint16_t terminalWidth() const = 0;
    virtual uint16_t terminalHeight() const = 0;
    
    static std::unique_ptr<Backend> create();
};

}
}