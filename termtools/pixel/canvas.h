#pragma once
#include "buffer.h"
#include "backend/backend.h"
#include <vector>
#include <memory>
#include <algorithm>

namespace tt {
namespace pixel {

class Canvas {
public:
    Canvas() : backend_(Backend::create()), cell_width_(1), cell_height_(2) {}

    void init() { backend_->init(); }
    void shutdown() { backend_->shutdown(); }
    void clear() { backend_->clear(); }
    void flush() { backend_->flush(); }

    void beginBatch() { backend_->beginBatch(); }
    void endBatch() { backend_->endBatch(); }

    void setCellSize(uint16_t cw, uint16_t ch) {
        cell_width_ = cw;
        cell_height_ = ch;
    }

    float aspectRatio() const { return static_cast<float>(cell_height_) / cell_width_; }

    void setPixel(int16_t x, int16_t y, char ch, Color fg, Color bg, uint8_t flags = FLAG_NONE) {
        if (x < 0 || y < 0) return;
        Pixel p;
        p.x = static_cast<uint16_t>(x);
        p.y = static_cast<uint16_t>(y);
        p.ch = ch;
        p.fg = fg;
        p.bg = bg;
        p.flags = flags;
        p.z_index = 0;
        backend_->setPixel(p.x, p.y, p);
    }

    void setPixel(int16_t x, int16_t y, const Pixel& pixel) {
        if (x < 0 || y < 0) return;
        Pixel p = pixel;
        p.x = static_cast<uint16_t>(x);
        p.y = static_cast<uint16_t>(y);
        backend_->setPixel(p.x, p.y, p);
    }

    void drawCircle(int16_t cx, int16_t cy, int16_t r, char ch, Color fg, Color bg, int16_t z = 0) {
        (void)z;
        float aspect = aspectRatio();
        int16_t x = r;
        int16_t y = 0;
        int16_t err = 0;

        while (x >= 0) {
            setPixel(cx + x, cy + static_cast<int16_t>(y * aspect), ch, fg, bg, FLAG_NONE);
            setPixel(cx - x, cy + static_cast<int16_t>(y * aspect), ch, fg, bg, FLAG_NONE);
            setPixel(cx + x, cy - static_cast<int16_t>(y * aspect), ch, fg, bg, FLAG_NONE);
            setPixel(cx - x, cy - static_cast<int16_t>(y * aspect), ch, fg, bg, FLAG_NONE);
            setPixel(cx + static_cast<int16_t>(y * aspect), cy + x, ch, fg, bg, FLAG_NONE);
            setPixel(cx - static_cast<int16_t>(y * aspect), cy + x, ch, fg, bg, FLAG_NONE);
            setPixel(cx + static_cast<int16_t>(y * aspect), cy - x, ch, fg, bg, FLAG_NONE);
            setPixel(cx - static_cast<int16_t>(y * aspect), cy - x, ch, fg, bg, FLAG_NONE);

            err += y * 2 + 1;
            y++;
            if (err < 0) {
                x--;
                err += x * 2 + 1;
            }
        }
    }

    void drawCircleFilled(int16_t cx, int16_t cy, int16_t r, char ch, Color fg, Color bg, int16_t z = 0) {
        (void)z;
        float aspect = aspectRatio();
        int16_t ra = static_cast<int16_t>(r * aspect);
        for (int16_t dy = -ra; dy <= ra; dy++) {
            for (int16_t dx = -r; dx <= r; dx++) {
                if (dx * dx + (dy * dy) / (aspect * aspect) <= r * r) {
                    setPixel(cx + dx, cy + dy, ch, fg, bg, FLAG_NONE);
                }
            }
        }
    }

    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, char ch, Color fg, Color bg, int16_t z = 0) {
        (void)z;
        float aspect = aspectRatio();
        int16_t dx = abs(x1 - x0);
        int16_t dy = abs(static_cast<int16_t>((y1 - y0) * aspect));
        int16_t sx = x0 < x1 ? 1 : -1;
        int16_t sy = (y0 * aspect) < (y1 * aspect) ? 1 : -1;
        int16_t err = dx - dy;
        int16_t y = static_cast<int16_t>(y0 * aspect);

        while (true) {
            setPixel(x0, static_cast<int16_t>(y / aspect), ch, fg, bg, FLAG_NONE);
            if (x0 == x1 && y == static_cast<int16_t>(y1 * aspect)) break;
            int16_t e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }

    uint16_t width() const { return backend_->terminalWidth(); }
    uint16_t height() const { return backend_->terminalHeight(); }

private:
    std::unique_ptr<Backend> backend_;
    uint16_t cell_width_;
    uint16_t cell_height_;
};

}
}