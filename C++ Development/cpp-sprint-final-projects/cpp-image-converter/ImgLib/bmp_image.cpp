#include "bmp_image.h"
#include "pack_defines.h"

#include <fstream>
#include <vector>
#include <iostream>

using namespace std;

namespace img_lib {

PACKED_STRUCT_BEGIN BitmapFileHeader {
    char signature[2] = {'B', 'M'};
    uint32_t file_size;
    uint32_t reserved = 0;
    uint32_t data_offset;
}
PACKED_STRUCT_END;

PACKED_STRUCT_BEGIN BitmapInfoHeader {
    uint32_t header_size = 40;
    int32_t width;
    int32_t height;
    uint16_t planes = 1;
    uint16_t bit_count = 24;
    uint32_t compression = 0;
    uint32_t image_size;
    int32_t x_pixels_per_meter = 11811;
    int32_t y_pixels_per_meter = 11811;
    uint32_t colors_used = 0;
    uint32_t important_colors = 0x1000000;
}
PACKED_STRUCT_END;

static int GetBMPStride(int w) {
    return 4 * ((w * 3 + 3) / 4);
}

bool SaveBMP(const Path& file, const Image& image) {
    ofstream ofs(file, ios::binary);
    if (!ofs) {
        cerr << "Error: Cannot open file for writing: " << file << endl;
        return false;
    }

    int width = image.GetWidth();
    int height = image.GetHeight();
    int stride = GetBMPStride(width);
    int data_size = stride * height;

    BitmapFileHeader file_header;
    file_header.data_offset = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    file_header.file_size = file_header.data_offset + data_size;
    
    BitmapInfoHeader info_header;
    info_header.width = width;
    info_header.height = height;
    info_header.image_size = data_size;
    
    ofs.write(reinterpret_cast<const char*>(&file_header), sizeof(file_header));
    ofs.write(reinterpret_cast<const char*>(&info_header), sizeof(info_header));

    vector<char> padding(stride - width * 3, 0);

    for (int y = height - 1; y >= 0; --y) {
        const Color* row = image.GetLine(y);
        for (int x = 0; x < width; ++x) {
            ofs.put(static_cast<char>(row[x].b));
            ofs.put(static_cast<char>(row[x].g));
            ofs.put(static_cast<char>(row[x].r));
        }
        ofs.write(padding.data(), padding.size());
    }

    return ofs.good();
}

Image LoadBMP(const Path& file) {
    ifstream ifs(file, ios::binary);
    if (!ifs) {
        cerr << "Error: Cannot open file for reading: " << file << endl;
        return {};
    }

    BitmapFileHeader file_header;
    BitmapInfoHeader info_header;

    ifs.read(reinterpret_cast<char*>(&file_header), sizeof(file_header));
    ifs.read(reinterpret_cast<char*>(&info_header), sizeof(info_header));

    if (file_header.signature[0] != 'B' || file_header.signature[1] != 'M' ||
        info_header.bit_count != 24 || info_header.compression != 0) {
        cerr << "Error: Unsupported BMP format" << endl;
        return {};
    }

    int width = info_header.width;
    int height = info_header.height;
    int stride = GetBMPStride(width);

    Image image(width, height, Color::Black());
    vector<char> padding(stride - width * 3);

    for (int y = height - 1; y >= 0; --y) {
        Color* row = image.GetLine(y);
        for (int x = 0; x < width; ++x) {
            char b, g, r;
            ifs.get(b).get(g).get(r);
            row[x] = {static_cast<byte>(r), static_cast<byte>(g), static_cast<byte>(b), byte{255}};
        }
        ifs.read(padding.data(), padding.size());
    }

    return image;
}

}  // namespace img_lib