#include "../pixel.h"
#include <vector>
#include <random>
#include <thread>
#include <chrono>

using namespace tt::pixel;

class GameOfLife {
public:
    GameOfLife(uint16_t width, uint16_t height) : width_(width), height_(height) {
        grid_.resize(height_, std::vector<bool>(width_, false));
        next_.resize(height_, std::vector<bool>(width_, false));
    }

    void randomize(float density = 0.3f) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);

        for (uint16_t y = 0; y < height_; y++) {
            for (uint16_t x = 0; x < width_; x++) {
                grid_[y][x] = dis(gen) < density;
            }
        }
    }

    void step() {
        for (uint16_t y = 0; y < height_; y++) {
            for (uint16_t x = 0; x < width_; x++) {
                int neighbors = countNeighbors(x, y);
                if (grid_[y][x]) {
                    next_[y][x] = (neighbors == 2 || neighbors == 3);
                } else {
                    next_[y][x] = (neighbors == 3);
                }
            }
        }
        grid_.swap(next_);
    }

    void draw(Canvas& canvas) {
        for (uint16_t y = 0; y < height_; y++) {
            for (uint16_t x = 0; x < width_; x++) {
                if (grid_[y][x]) {
                    canvas.setPixel(x, y, 'O', Color::green(), Color::gray(10));
                }
            }
        }
    }

private:
    int countNeighbors(uint16_t x, uint16_t y) {
        int count = 0;
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                if (dx == 0 && dy == 0) continue;
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < (int)width_ && ny >= 0 && ny < (int)height_) {
                    if (grid_[ny][nx]) count++;
                }
            }
        }
        return count;
    }

    uint16_t width_, height_;
    std::vector<std::vector<bool>> grid_, next_;
};

int main() {
    Canvas canvas;
    canvas.init();

    canvas.setCellSize(1, 2);

    GameOfLife gol(78, 21);
    gol.randomize();

    while (true) {
        canvas.clear();

        TextPrimitive title(2, 0, "Conway's Game of Life");
        title.setStyle(Color::white(), Color::black(), FLAG_BOLD);
        title.draw(canvas);

        gol.draw(canvas);

        canvas.flush();
        gol.step();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    canvas.shutdown();
    return 0;
}
