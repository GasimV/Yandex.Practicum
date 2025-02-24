#include "ppm_image.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <string_view>

using namespace std;

int Sum(img_lib::Color c) {
    return to_integer<int>(c.r) + to_integer<int>(c.g) + to_integer<int>(c.b);
}

// Функция для оператора Собеля
img_lib::Image Sobel(const img_lib::Image& image) {
    const int width = image.GetWidth();
    const int height = image.GetHeight();
    img_lib::Image result(width, height, img_lib::Color::Black());

    // Пройтись по всем пикселям, кроме границ
    for (int y = 1; y < height - 1; ++y) {
        img_lib::Color* dst_line = result.GetLine(y);
        const img_lib::Color* prev = image.GetLine(y - 1);
        const img_lib::Color* curr = image.GetLine(y);
        const img_lib::Color* next = image.GetLine(y + 1);

        for (int x = 1; x < width - 1; ++x) {
            int tl = Sum(prev[x - 1]), tc = Sum(prev[x]), tr = Sum(prev[x + 1]);
            int cl = Sum(curr[x - 1]), cr = Sum(curr[x + 1]);
            int bl = Sum(next[x - 1]), bc = Sum(next[x]), br = Sum(next[x + 1]);

            // Вычисление gx и gy
            int gx = -tl - 2 * tc - tr + bl + 2 * bc + br;
            int gy = -tl - 2 * cl - bl + tr + 2 * cr + br;

            // Вычисляем градиент
            double gradient = sqrt(gx * gx + gy * gy);
            std::byte value = static_cast<std::byte>(clamp(gradient, 0.0, 255.0));

            // Записываем одинаковые компоненты в R, G, B
            dst_line[x] = {value, value, value, curr[x].a};
        }
    }

    return result;
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input image> <output image>"sv << endl;
        return 1;
    }

    auto image = img_lib::LoadPPM(argv[1]);
    if (!image) {
        cerr << "Error loading image"sv << endl;
        return 2;
    }

    image = Sobel(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}