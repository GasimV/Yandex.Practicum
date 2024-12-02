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

int main() {
    using namespace shapes;

    vector<unique_ptr<svg::Drawable>> picture;
    picture.emplace_back(make_unique<Triangle>(Point{100, 20}, Point{120, 50}, Point{80, 40}));
    picture.emplace_back(make_unique<Star>(Point{50.0, 20.0}, 10.0, 4.0, 5));
    picture.emplace_back(make_unique<Snowman>(Point{30, 20}, 10.0));

    svg::Document doc;
    DrawPicture(picture, doc);

    const Text base_text =  //
        Text()
            .SetFontFamily("Verdana"s)
            .SetFontSize(12)
            .SetPosition({10, 100})
            .SetData("Happy New Year!"s);
    doc.Add(Text{base_text}
                .SetStrokeColor("yellow"s)
                .SetFillColor("yellow"s)
                .SetStrokeLineJoin(StrokeLineJoin::ROUND)
                .SetStrokeLineCap(StrokeLineCap::ROUND)
                .SetStrokeWidth(3));
    doc.Add(Text{base_text}.SetFillColor("red"s));

    doc.Render(cout);
}