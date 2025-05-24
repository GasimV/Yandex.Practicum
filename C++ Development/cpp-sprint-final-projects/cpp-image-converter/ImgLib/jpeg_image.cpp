#include "jpeg_image.h"
#include <vector>
#include <cstdio>
#include <setjmp.h>
#include <jpeglib.h>
#include <iostream>

using namespace std;

namespace img_lib {

// Структура для обработки ошибок в libjpeg
struct my_error_mgr {
    jpeg_error_mgr pub;
    jmp_buf setjmp_buffer;
};

typedef my_error_mgr* my_error_ptr;

// Обработчик ошибок
METHODDEF(void) my_error_exit(j_common_ptr cinfo) {
    my_error_ptr myerr = (my_error_ptr)cinfo->err;
    longjmp(myerr->setjmp_buffer, 1);
}

// Функция сохранения изображения в формате JPEG
bool SaveJPEG(const Path& file, const Image& image) {
    jpeg_compress_struct cinfo;
    my_error_mgr jerr;
    
    FILE* outfile = nullptr;
    
#ifdef _MSC_VER
    if ((outfile = _wfopen(file.wstring().c_str(), L"wb")) == nullptr) {
#else
    if ((outfile = fopen(file.string().c_str(), "wb")) == nullptr) {
#endif
        return false;
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;
    
    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_compress(&cinfo);
        fclose(outfile);
        return false;
    }

    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = image.GetWidth();
    cinfo.image_height = image.GetHeight();
    cinfo.input_components = 3;  // RGB
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = cinfo.image_width * 3;
    vector<JSAMPLE> buffer(row_stride);

    while (cinfo.next_scanline < cinfo.image_height) {
        const Color* line = image.GetLine(cinfo.next_scanline);

        for (int x = 0; x < cinfo.image_width; ++x) {
            buffer[x * 3]     = static_cast<JSAMPLE>(to_integer<int>(line[x].r));
            buffer[x * 3 + 1] = static_cast<JSAMPLE>(to_integer<int>(line[x].g));
            buffer[x * 3 + 2] = static_cast<JSAMPLE>(to_integer<int>(line[x].b));
        }

        JSAMPROW row_pointer = buffer.data();
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);

    return true;
}

// Функция загрузки JPEG
Image LoadJPEG(const Path& file) {
    jpeg_decompress_struct cinfo;
    my_error_mgr jerr;
    FILE* infile = nullptr;

#ifdef _MSC_VER
    infile = _wfopen(file.wstring().c_str(), L"rb");
#else
    infile = fopen(file.string().c_str(), "rb");
#endif

    if (!infile) {
        cerr << "Error: Cannot open file " << file << endl;
        return {};
    }

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer)) {
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return {};
    }

    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int channels = cinfo.output_components;

    if (channels != 3) {
        cerr << "Error: Unsupported JPEG format (only RGB supported)" << endl;
        jpeg_destroy_decompress(&cinfo);
        fclose(infile);
        return {};
    }

    Image image(width, height, Color::Black());
    vector<JSAMPLE> buffer(width * channels);
    while (cinfo.output_scanline < height) {
        JSAMPROW row_pointer = buffer.data();
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);

        Color* line = image.GetLine(cinfo.output_scanline - 1);
        for (int x = 0; x < width; ++x) {
            line[x] = {static_cast<byte>(buffer[x * 3]),
                       static_cast<byte>(buffer[x * 3 + 1]),
                       static_cast<byte>(buffer[x * 3 + 2]),
                       byte{255}};
        }
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);

    return image;
}

} // namespace img_lib
