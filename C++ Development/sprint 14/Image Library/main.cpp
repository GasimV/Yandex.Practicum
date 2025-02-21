#include "ppm_image.h"
#include <iostream>
#include <string_view>
#include <cstddef> // для std::byte и std::to_integer

using namespace std;

// Функция для обращения одной компоненты цвета
std::byte Negate(std::byte c) {
    return static_cast<std::byte>(255 - std::to_integer<int>(c));
}

// Функция для обращения цвета
img_lib::Color Negate(img_lib::Color c) {
    return {Negate(c.r), Negate(c.g), Negate(c.b), c.a};
}

// Функция для обращения цветов в изображении
void NegateInplace(img_lib::Image& image) {
    const int height = image.GetHeight();
    const int width = image.GetWidth();

    for (int y = 0; y < height; ++y) {
        img_lib::Color* line = image.GetLine(y);
        for (int x = 0; x < width; ++x) {
            line[x] = Negate(line[x]);
        }
    }
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

    // Применяем функцию NegateInplace перед сохранением
    NegateInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}