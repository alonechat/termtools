#include "../pixel.h"
#include <cmath>
#include <ctime>
#include <thread>
#include <chrono>

using namespace tt::pixel;

const double PI = 3.14159265358979323846;

class AnalogClock {
public:
    AnalogClock(int16_t cx, int16_t cy, int16_t radius)
        : cx_(cx), cy_(cy), radius_(radius) {}

    void drawHands(Canvas& canvas, double hours, double minutes, double seconds) {
        float aspect = canvas.aspectRatio();

        int16_t hx = cx_ + static_cast<int16_t>(6 * sin(hours));
        int16_t hy = cy_ - static_cast<int16_t>(6 * cos(hours) / aspect);
        canvas.drawLine(cx_, cy_, hx, hy, 'H', Color::red(), Color::black());

        int16_t mx = cx_ + static_cast<int16_t>(8 * sin(minutes));
        int16_t my = cy_ - static_cast<int16_t>(8 * cos(minutes) / aspect);
        canvas.drawLine(cx_, cy_, mx, my, 'M', Color::green(), Color::black());

        int16_t sx = cx_ + static_cast<int16_t>(9 * sin(seconds));
        int16_t sy = cy_ - static_cast<int16_t>(9 * cos(seconds) / aspect);
        canvas.drawLine(cx_, cy_, sx, sy, 'S', Color::white(), Color::black());
    }

    void drawFace(Canvas& canvas) {
        canvas.drawCircle(cx_, cy_, radius_, '#', Color::cyan(), Color::black());
        canvas.drawCircle(cx_, cy_, radius_ - 1, '*', Color::gray(80), Color::black());

        float aspect = canvas.aspectRatio();
        for (int i = 0; i < 12; i++) {
            double angle = i * (2 * PI / 12);
            int16_t mx = cx_ + static_cast<int16_t>((radius_ - 2) * sin(angle));
            int16_t my = cy_ - static_cast<int16_t>((radius_ - 2) * cos(angle) / aspect);
            canvas.setPixel(mx, my, '0' + (i % 10), Color::yellow(), Color::black(), 3);
        }
    }

private:
    int16_t cx_, cy_, radius_;
};

int main() {
    Canvas canvas;
    canvas.init();

    canvas.setCellSize(1, 2);

    AnalogClock clock(40, 10, 10);

    while (true) {
        time_t now = time(nullptr);
        struct tm* t = localtime(&now);

        double hours = (t->tm_hour % 12) * (2 * PI / 12) + (t->tm_min / 60.0) * (2 * PI / 12);
        double minutes = t->tm_min * (2 * PI / 60) + (t->tm_sec / 60.0) * (2 * PI / 60);
        double seconds = t->tm_sec * (2 * PI / 60);

        canvas.clear();

        clock.drawFace(canvas);
        clock.drawHands(canvas, hours, minutes, seconds);

        TextPrimitive timeStr(2, 0, "Analog Clock (Aspect Corrected)");
        timeStr.setStyle(Color::white(), Color::black(), FLAG_BOLD);
        timeStr.draw(canvas);

        canvas.flush();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    canvas.shutdown();
    return 0;
}
