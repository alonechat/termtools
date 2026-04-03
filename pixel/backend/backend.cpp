#include "ansi_backend.h"
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

namespace tt {
namespace pixel {

std::unique_ptr<Backend> Backend::create() {
    return std::make_unique<ANSIBackend>();
}

ANSIBackend::ANSIBackend() 
    : terminal_width_(80)
    , terminal_height_(24)
    , last_cursor_x_(0)
    , last_cursor_y_(0)
    , needs_full_refresh_(true) {
}

ANSIBackend::~ANSIBackend() {
    shutdown();
}

void ANSIBackend::init() {
    printf("\033[?1049h");
    printf("\033[2J");
    printf("\033[?25l");
    fflush(stdout);
    updateTerminalSize();
}

void ANSIBackend::shutdown() {
    printf("\033[?1049l");
    printf("\033[?25h");
    fflush(stdout);
}

void ANSIBackend::clear() {
    printf("\033[2J");
    needs_full_refresh_ = true;
}

void ANSIBackend::flush() {
    if (!output_buffer_.empty()) {
        fwrite(output_buffer_.data(), 1, output_buffer_.size(), stdout);
        output_buffer_.clear();
    }
    fflush(stdout);
    needs_full_refresh_ = false;
}

void ANSIBackend::beginBatch() {
}

void ANSIBackend::endBatch() {
    flush();
}

void ANSIBackend::setPixel(uint16_t x, uint16_t y, const Pixel& pixel) {
    if (x >= terminal_width_ || y >= terminal_height_) return;
    
    if (needs_full_refresh_) {
        writeANSICode(pixel);
        char buf[32];
        int len = snprintf(buf, sizeof(buf), "\033[%d;%dH%c", y + 1, x + 1, pixel.ch);
        output_buffer_.append(buf, len);
    } else {
        pending_pixels_.push_back({x, y, pixel});
    }
}

void ANSIBackend::moveCursor(uint16_t x, uint16_t y) {
    char buf[16];
    int len = snprintf(buf, sizeof(buf), "\033[%d;%dH", y + 1, x + 1);
    output_buffer_.append(buf, len);
    last_cursor_x_ = x;
    last_cursor_y_ = y;
}

void ANSIBackend::writeANSICode(const Pixel& pixel) {
    if (pixel.fg.alpha == 0 && pixel.bg.alpha == 0) return;
    
    output_buffer_ += "\033[";
    
    bool has_sgr = false;
    
    if (pixel.fg.alpha > 0) {
        if (pixel.fg.r == pixel.fg.g && pixel.fg.g == pixel.fg.b) {
            char buf[16];
            int len = snprintf(buf, sizeof(buf), "38;5;%d", pixel.fg.r);
            output_buffer_.append(buf, len);
            has_sgr = true;
        } else {
            char buf[32];
            int len = snprintf(buf, sizeof(buf), "38;2;%d;%d;%d", pixel.fg.r, pixel.fg.g, pixel.fg.b);
            output_buffer_.append(buf, len);
            has_sgr = true;
        }
    }
    
    if (pixel.bg.alpha > 0) {
        if (has_sgr) output_buffer_ += ';';
        if (pixel.bg.r == pixel.bg.g && pixel.bg.g == pixel.bg.b) {
            char buf[16];
            int len = snprintf(buf, sizeof(buf), "48;5;%d", pixel.bg.r);
            output_buffer_.append(buf, len);
            has_sgr = true;
        } else {
            char buf[32];
            int len = snprintf(buf, sizeof(buf), "48;2;%d;%d;%d", pixel.bg.r, pixel.bg.g, pixel.bg.b);
            output_buffer_.append(buf, len);
            has_sgr = true;
        }
    }
    
    if (pixel.flags & FLAG_BOLD) {
        if (has_sgr) output_buffer_ += ';';
        output_buffer_ += "1";
        has_sgr = true;
    }
    if (pixel.flags & FLAG_ITALIC) {
        if (has_sgr) output_buffer_ += ';';
        output_buffer_ += "3";
        has_sgr = true;
    }
    if (pixel.flags & FLAG_UNDERLINE) {
        if (has_sgr) output_buffer_ += ';';
        output_buffer_ += "4";
        has_sgr = true;
    }
    
    output_buffer_ += "m";
}

void ANSIBackend::flushPixel(const PendingPixel& pp) {
    if (pp.x != last_cursor_x_ || pp.y != last_cursor_y_) {
        moveCursor(pp.x, pp.y);
    }
    writeANSICode(pp.pixel);
    char buf[8];
    int len = snprintf(buf, sizeof(buf), "%c", pp.pixel.ch);
    output_buffer_.append(buf, len);
    last_cursor_x_ = pp.x + 1;
}

void ANSIBackend::updateTerminalSize() {
#ifdef TIOCGWINSZ
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        if (ws.ws_col > 0) terminal_width_ = ws.ws_col;
        if (ws.ws_row > 0) terminal_height_ = ws.ws_row;
    }
#endif
}

}
}