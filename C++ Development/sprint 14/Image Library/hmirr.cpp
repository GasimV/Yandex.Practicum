#include "ppm_image.h"

#include <iostream>
#include <string_view>

using namespace std;

// Функция для горизонтального отражения
void HMirrInplace(img_lib::Image& image) {
    const int width = image.GetWidth();
    const int height = image.GetHeight();

    for (int y = 0; y < height; ++y) {
        img_lib::Color* line = image.GetLine(y);
        for (int x = 0; x < width / 2; ++x) {
            swap(line[x], line[width - 1 - x]);
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

    HMirrInplace(image);

    if (!img_lib::SavePPM(argv[2], image)) {
        cerr << "Error saving image"sv << endl;
        return 3;
    }

    cout << "Image saved successfully!"sv << endl;
}