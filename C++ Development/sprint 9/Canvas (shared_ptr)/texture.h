#pragma once
#include "common.h"

class Texture {
public:
    explicit Texture(Image image)
        : image_(std::move(image)) {
    }

    Size GetSize() const {
        return GetImageSize(image_);
    }

    char GetPixelColor(Point p) const {
        Size size = GetSize();
        if (p.x >= 0 && p.x < size.width && p.y >=0 && p.y < size.height) {
            return image_[p.y][p.x];
        }
        return ' ';
    }

private:
    Image image_;
};