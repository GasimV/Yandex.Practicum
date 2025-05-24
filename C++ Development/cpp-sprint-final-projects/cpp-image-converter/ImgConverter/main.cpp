#include <img_lib.h>
#include <jpeg_image.h>
#include <ppm_image.h>
#include <bmp_image.h>
#include <filesystem>
#include <string_view>
#include <iostream>

using namespace std;
using namespace img_lib;

enum class Format { BMP, JPEG, PPM, UNKNOWN };

Format GetFormatByExtension(const Path& file) {
    const string ext = file.extension().string();
    if (ext == ".bmp"sv) return Format::BMP;
    if (ext == ".jpg"sv || ext == ".jpeg"sv) return Format::JPEG;
    if (ext == ".ppm"sv) return Format::PPM;
    return Format::UNKNOWN;
}

Image LoadImage(const Path& file) {
    switch (GetFormatByExtension(file)) {
        case Format::BMP:   return LoadBMP(file);
        case Format::PPM:   return LoadPPM(file);
        case Format::JPEG:  return LoadJPEG(file);
        default:
            cerr << "Unknown format of the input file"sv << endl;
            return {};
    }
}

bool SaveImage(const Path& file, const Image& image) {
    switch (GetFormatByExtension(file)) {
        case Format::BMP:   return SaveBMP(file, image);
        case Format::PPM:   return SavePPM(file, image);
        case Format::JPEG:  return SaveJPEG(file, image);
        default:
            cerr << "Unknown format of the output file"sv << endl;
            return false;
    }
}

int main(int argc, const char** argv) {
    if (argc != 3) {
        cerr << "Usage: "sv << argv[0] << " <input file> <output file>"sv << endl;
        return 1;
    }

    Path in_path = argv[1];
    Path out_path = argv[2];

    Image image = LoadImage(in_path);
    if (!image) {
        cerr << "Cannot load image"sv << endl;
        return 4;
    }

    if (!SaveImage(out_path, image)) {
        cerr << "Cannot save image"sv << endl;
        return 5;
    }

    cout << "Successfully converted"sv << endl;
    return 0;
}