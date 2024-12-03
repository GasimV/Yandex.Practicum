#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

// Структура, представляющая цвет RGB
struct Rgb {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
};

// Структура, представляющая цвет RGBA
struct Rgba {
    uint8_t red = 0;
    uint8_t green = 0;
    uint8_t blue = 0;
    double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor{"none"};

// Перечисление Enum, представляющее различные стили окончания строки
enum class StrokeLineCap {
    BUTT,
    ROUND,
    SQUARE
};

// Перечисление Enum, представляющее различные стили соединения линий
enum class StrokeLineJoin {
    ARCS,
    BEVEL,
    MITER,
    MITER_CLIP,
    ROUND,
};

// Перегрузить оператор для вывода StrokeLineCap в виде строки
std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap);

// Перегрузить оператор для вывода StrokeLineJoin в виде строки
std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join);

// Перегруженный оператор для вывода цвета в виде строки
std::ostream& operator<<(std::ostream& out, const Color& color);

// Структура, представляющая точку в двумерном пространстве
struct Point {
    Point() = default;
    Point(double x, double y)
        : x(x)
        , y(y) {
    }
    double x = 0;
    double y = 0;
};

// Структура для управления контекстом рендеринга с отступами
struct RenderContext {
    RenderContext(std::ostream& out)
        : out(out) {
    }

    RenderContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
    }

    // Возвращает новый RenderContext с увеличенным отступом
    RenderContext Indented() const {
        return {out, indent_step, indent + indent_step};
    }

    // Отображает текущий уровень отступа
    void RenderIndent() const {
        for (int i = 0; i < indent; ++i) {
            out.put(' ');
        }
    }

    std::ostream& out;
    int indent_step = 0;
    int indent = 0;
};

// Абстрактный базовый класс для всех объектов SVG
class Object {
public:
    void Render(const RenderContext& context) const;

    virtual ~Object() = default;

private:
    virtual void RenderObject(const RenderContext& context) const = 0;
};

// Абстрактный базовый класс для контейнера объектов SVG
class ObjectContainer {
public:
    virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;

    template <typename Obj>
    void Add(Obj obj) {
        AddPtr(std::make_unique<Obj>(std::move(obj)));
    }

    virtual ~ObjectContainer() = default;
};

// Интерфейс для рисуемых объектов
class Drawable {
public:
    virtual void Draw(ObjectContainer& container) const = 0;
    virtual ~Drawable() = default;
};

// Класс для управления свойствами пути, такими как цвет заливки, цвет обводки и т. д.
template <typename Owner>
class PathProps {
private:
    std::optional<Color> fill_color_;
    std::optional<Color> stroke_color_;
    std::optional<double> stroke_width_;
    std::optional<StrokeLineCap> stroke_linecap_;
    std::optional<StrokeLineJoin> stroke_linejoin_;

public:
    Owner& SetFillColor(Color color) {
        fill_color_ = std::move(color);
        return static_cast<Owner&>(*this);
    }

    Owner& SetStrokeColor(Color color) {
        stroke_color_ = std::move(color);
        return static_cast<Owner&>(*this);
    }

    Owner& SetStrokeWidth(double width) {
        stroke_width_ = width;
        return static_cast<Owner&>(*this);
    }

    Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
        stroke_linecap_ = line_cap;
        return static_cast<Owner&>(*this);
    }

    Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
        stroke_linejoin_ = line_join;
        return static_cast<Owner&>(*this);
    }

protected:
    // Отображает свойства пути как атрибуты в теге SVG
    void RenderAttrs(std::ostream& out) const {
        if (fill_color_) {
            out << " fill=\"" << *fill_color_ << "\"";
        }

        if (stroke_color_) {
            out << " stroke=\"" << *stroke_color_ << "\"";
        }

        if (stroke_width_) {
            out << " stroke-width=\"" << *stroke_width_ << "\"";
        }

        if (stroke_linecap_) {
            out << " stroke-linecap=\"" << *stroke_linecap_ << "\"";
        }

        if (stroke_linejoin_) {
            out << " stroke-linejoin=\"" << *stroke_linejoin_ << "\"";
        }
    }
};

// Класс, представляющий круг SVG
class Circle final : public Object, public PathProps<Circle> {
public:
    Circle& SetCenter(Point center);
    Circle& SetRadius(double radius);

private:
    void RenderObject(const RenderContext& context) const override;

    Point center_;
    double radius_ = 1.0;
};

// Класс, представляющий ломаную SVG-линию
class Polyline final : public Object, public PathProps<Polyline> {
public:
    // Добавляет очередную вершину к ломаной линии
    Polyline& AddPoint(Point point);

private:
    void RenderObject(const RenderContext& context) const override;

    std::vector<Point> points_;
};

// Класс, представляющий текстовый элемент SVG
class Text final : public Object, public PathProps<Text> {
public:
    // Задаёт координаты опорной точки (атрибуты x и y)
    Text& SetPosition(Point pos);

    // Задаёт смещение относительно опорной точки (атрибуты dx, dy)
    Text& SetOffset(Point offset);

    // Задаёт размеры шрифта (атрибут font-size)
    Text& SetFontSize(uint32_t size);

    // Задаёт название шрифта (атрибут font-family)
    Text& SetFontFamily(std::string font_family);

    // Задаёт толщину шрифта (атрибут font-weight)
    Text& SetFontWeight(std::string font_weight);

    // Задаёт текстовое содержимое объекта (отображается внутри тега text)
    Text& SetData(std::string data);

private:
    void RenderObject(const RenderContext& context) const override;

    Point position_;
    Point offset_;
    uint32_t font_size_ = 1;
    std::string font_family_;
    std::string font_weight_;
    std::string data_;
};

// Класс, представляющий документ SVG, который может содержать несколько объектов SVG
class Document : public ObjectContainer {
public:
    // Добавляет в svg-документ объект-наследник svg::Object
    void AddPtr(std::unique_ptr<Object>&& obj) override;

    // Выводит в ostream svg-представление документа
    void Render(std::ostream& out) const;

private:
    std::vector<std::unique_ptr<Object>> objects_;
};

}  // namespace svg