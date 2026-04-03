#pragma once
#include "backend.h"
#include <vector>
#include <string>

namespace tt {
namespace pixel {

class ANSIBackend : public Backend {
public:
    ANSIBackend();
    ~ANSIBackend() override;
    
    void init() override;
    void shutdown() override;
    void clear() override;
    void flush() override;
    void setPixel(uint16_t x, uint16_t y, const Pixel& pixel) override;
    void moveCursor(uint16_t x, uint16_t y) override;
    void beginBatch() override;
    void endBatch() override;
    
    uint16_t terminalWidth() const override { return terminal_width_; }
    uint16_t terminalHeight() const override { return terminal_height_; }
    
private:
    uint16_t terminal_width_;
    uint16_t terminal_height_;
    
    struct PendingPixel {
        uint16_t x, y;
        Pixel pixel;
    };
    std::vector<PendingPixel> pending_pixels_;
    
    std::string output_buffer_;
    uint16_t last_cursor_x_;
    uint16_t last_cursor_y_;
    
    bool needs_full_refresh_;
    
    void writeANSICode(const Pixel& pixel);
    void flushPixel(const PendingPixel& pp);
    void updateTerminalSize();
};

}
}