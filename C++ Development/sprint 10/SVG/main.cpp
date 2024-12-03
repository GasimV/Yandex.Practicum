#define _USE_MATH_DEFINES
#include "svg.h"

#include <cmath>
#include <vector>
#include <memory>

using namespace svg;
using namespace std;

namespace shapes {

// Класс, представляющий треугольник, производный от Drawable
class Triangle : public svg::Drawable {
private:
    svg::Point p1_, p2_, p3_;

public:
    Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
        : p1_(p1), p2_(p2), p3_(p3) {}
    
    // Рисуем треугольник, добавляя в контейнер ломаную линию
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
    }
};

// Класс, представляющий звезду, производный от Drawable
class Star : public svg::Drawable {
private:
    svg::Point center_;
    double outer_radius_;
    double inner_radius_;
    int num_rays_;

    // Вспомогательная функция для создания ломаной линии, представляющей звезду
    svg::Polyline CreateStar(svg::Point center, double outer_radius, double inner_radius, int num_rays) const {
        svg::Polyline polyline;
        for (int i = 0; i <= num_rays; ++i) {
            double angle = 2 * M_PI * (i % num_rays) / num_rays;
            polyline.AddPoint({center.x + outer_radius * sin(angle), center.y - outer_radius * cos(angle)});
            if (i == num_rays) {
                break;
            }
            angle += M_PI / num_rays;
            polyline.AddPoint({center.x + inner_radius * sin(angle), center.y - inner_radius * cos(angle)});
        }
        return polyline;
    }

public:
    Star(svg::Point center, double outer_radius, double inner_radius, int num_rays)
        : center_(center), outer_radius_(outer_radius), inner_radius_(inner_radius), num_rays_(num_rays) {}

    // Рисуем звезду, добавляя в контейнер ломаную линию определенных цветов
    void Draw(svg::ObjectContainer& container) const override {
        container.Add(CreateStar(center_, outer_radius_, inner_radius_, num_rays_)
                        .SetFillColor("red")
                        .SetStrokeColor("black"));
    }
};

// Класс, представляющий снеговика, производный от Drawable
class Snowman : public svg::Drawable {
private:
    svg::Point head_center_;
    double head_radius_;

public:
    Snowman(svg::Point head_center, double head_radius) : head_center_(head_center), head_radius_(head_radius) {}

    // Нарисуем снеговика, добавив три круга в контейнер
    void Draw(svg::ObjectContainer& container) const override {
        double body_radius = head_radius_ * 1.5;
        double base_radius = head_radius_ * 2;

        // Рисуем круги основания, тела и головы определенными цветами
        container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y + 2 * head_radius_ + 3 * head_radius_}).SetRadius(base_radius)
            .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(svg::Circle().SetCenter({head_center_.x, head_center_.y + 2 * head_radius_}).SetRadius(body_radius)
            .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
        container.Add(svg::Circle().SetCenter(head_center_).SetRadius(head_radius_)
            .SetFillColor("rgb(240,240,240)").SetStrokeColor("black"));
    }
};

} // namespace shapes

// Функция для рисования всех рисуемых объектов из диапазона итератора в целевой контейнер
template <typename DrawableIterator>
void DrawPicture(DrawableIterator begin, DrawableIterator end, svg::ObjectContainer& target) {
    for (auto it = begin; it != end; ++it) {
        (*it)->Draw(target);
    }
}

// Функция для рисования всех рисуемых объектов из контейнера в целевой контейнер
template <typename Container>
void DrawPicture(const Container& container, svg::ObjectContainer& target) {
    using namespace std;
    DrawPicture(begin(container), end(container), target);
}

// Функция для выполнения линейной интерполяции между двумя значениями
uint8_t Lerp(uint8_t from, uint8_t to, double t) {
    return static_cast<uint8_t>(std::round((to - from) * t + from));
}

int main() {
    // Определяем начальный и конечный цвета для градиента
    Rgb start_color{0, 255, 30}; // Ярко-зеленый
    Rgb end_color{20, 20, 150}; // Темно-синий

    const int num_circles = 10; // Количество кругов для рисования
    Document doc;
    for (int i = 0; i < num_circles; ++i) {
        // Рассчитаем коэффициент интерполяции (t) для каждого круга
        const double t = double(i) / (num_circles - 1);
        // Интерполируем цвет между start_color и end_color
        Rgb fill_color{
            static_cast<uint8_t>(Lerp(start_color.red, end_color.red, t)),
            static_cast<uint8_t>(Lerp(start_color.green, end_color.green, t)),
            static_cast<uint8_t>(Lerp(start_color.blue, end_color.blue, t))
        };

        // Добавляем в документ круг с интерполированным цветом
        doc.Add(Circle()
                    .SetFillColor(fill_color)
                    .SetStrokeColor("black"s) // Черный цвет обводки
                    .SetCenter({i * 20.0 + 40, 40.0}) // Располагаем каждый круг горизонтально
                    .SetRadius(15)); // Устанавливаем радиус круга
    }
    // Выводим документ на стандартный вывод
    doc.Render(cout);
}