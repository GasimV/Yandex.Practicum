#pragma once
#include "texture.h"

#include <memory>

// Поддерживаемые виды фигур: прямоугольник и эллипс
enum class ShapeType { RECTANGLE, ELLIPSE };

class Shape {
public:
    // Фигура после создания имеет нулевые координаты и размер,
    // а также не имеет текстуры
    explicit Shape(ShapeType type) 
        : shape_type_(type), top_left_angle_point_({0, 0}), shape_size_({0, 0}), texture_(nullptr) {
    }

    void SetPosition(Point pos) {
        top_left_angle_point_ = pos;
    }

    void SetSize(Size size) {
        shape_size_ = size;
    }

    void SetTexture(std::shared_ptr<Texture> texture) {
        texture_ = std::move(texture);
    }

    // Рисует фигуру на указанном изображении
    void Draw(Image& image) const {
        Size image_size = GetImageSize(image);
        Size texture_size = {0, 0};

        if (texture_ != nullptr) {
            texture_size = texture_->GetSize();
        }

        for (int y = 0; y < shape_size_.height; ++y) {
            for (int x = 0; x < shape_size_.width; ++x) {
                // Map shape-relative coordinates to absolute canvas coordinates
                int canvas_x = top_left_angle_point_.x + x;
                int canvas_y = top_left_angle_point_.y + y;

                // Ensure the pixel lies within the canvas bounds
                if (canvas_x < 0 || canvas_x >= image_size.width || canvas_y < 0 || canvas_y >= image_size.height) {
                    continue;
                }

                // Determine if the point lies within the shape
                bool inside = (shape_type_ == ShapeType::RECTANGLE) ||
                              (shape_type_ == ShapeType::ELLIPSE && IsPointInEllipse({x, y}, shape_size_));

                if (!inside) {
                    continue;
                }

                // Determine the pixel color
                char color = '.';
                if (texture_ && x < texture_size.width && y < texture_size.height) {
                    color = texture_->GetPixelColor({x, y});
                }

                // Draw the pixel on the canvas
                image[canvas_y][canvas_x] = color;
            }
        }
    }

private:
    ShapeType shape_type_;
    Point top_left_angle_point_;
    Size shape_size_;
    std::shared_ptr<Texture> texture_;
};