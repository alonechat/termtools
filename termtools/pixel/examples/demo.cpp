#include "../pixel.h"
#include <thread>
#include <chrono>
#include <cstdio>

using namespace tt::pixel;

int main() {
    Canvas canvas;
    canvas.init();

    canvas.setCellSize(1, 2);

    int frame = 0;
    while (true) {
        canvas.clear();

        TextPrimitive title(2, 0, "Aspect Ratio Corrected Drawing Demo (Cell: 1x2)");
        title.setStyle(Color::white(), Color::black(), FLAG_BOLD);
        title.draw(canvas);

        RectPrimitive rect1(2, 3, 12, 6);
        rect1.setBorder('#', Color::cyan(), Color::black());
        rect1.setFill(' ', Color::white(), Color::gray(20));
        rect1.draw(canvas);

        TextPrimitive labels1(2, 11, "Rect 12x6 (should be square)");
        labels1.setStyle(Color::gray(150), Color::black());
        labels1.draw(canvas);

        int16_t circleX = 35;
        int16_t circleY = 6;
        canvas.drawCircle(circleX, circleY, 5, '*', Color::green(), Color::black());

        TextPrimitive labels2(2, 14, "Circle r=5 (should be round)");
        labels2.setStyle(Color::gray(150), Color::black());
        labels2.draw(canvas);

        canvas.drawCircleFilled(circleX, circleY + 16, 5, '#', Color::magenta(), Color::gray(40));

        TextPrimitive labels3(2, 24, "Filled Circle (should be round)");
        labels3.setStyle(Color::gray(150), Color::black());
        labels3.draw(canvas);

        canvas.drawLine(2, 2, 75, 2, '=', Color::yellow(), Color::black());
        canvas.drawLine(2, 23, 75, 23, '=', Color::yellow(), Color::black());

        char buf[32];
        snprintf(buf, sizeof(buf), "Frame: %d", frame);
        TextPrimitive fps(70, 0, buf);
        fps.setStyle(Color::gray(128), Color::black());
        fps.draw(canvas);

        canvas.flush();
        frame++;

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    canvas.shutdown();
    return 0;
}
