#include "svg.h"

namespace svg {

using namespace std::literals;

// Перегруженный оператор для вывода StrokeLineCap в виде строки
std::ostream& operator<<(std::ostream& out, StrokeLineCap line_cap) {
    switch (line_cap) {
        case StrokeLineCap::BUTT: out << "butt"; break;
        case StrokeLineCap::ROUND: out << "round"; break;
        case StrokeLineCap::SQUARE: out << "square"; break;
    }
    return out;
}

// Перегруженный оператор для вывода StrokeLineJoin в виде строки
std::ostream& operator<<(std::ostream& out, StrokeLineJoin line_join) {
    switch (line_join) {
        case StrokeLineJoin::ARCS: out << "arcs"; break;
        case StrokeLineJoin::BEVEL: out << "bevel"; break;
        case StrokeLineJoin::MITER: out << "miter"; break;
        case StrokeLineJoin::MITER_CLIP: out << "miter-clip"; break;
        case StrokeLineJoin::ROUND: out << "round"; break;
    }
    return out;
}

// Метод Render для Object вызывает метод RenderObject для производных классов
void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

// Устанавливаем центральную точку круга
Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

// Устанавливаем радиус круга
Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

// Отобразить элемент круга как тег SVG
void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\"";
    RenderAttrs(out); // Отображение дополнительных атрибутов, таких как заливка, обводка и т. д.
    out << " />"sv;
}

// Добавляем точку к ломаной линии
Polyline& Polyline::AddPoint(Point point) {
    points_.emplace_back(point);
    return *this;
}

// Отрисовываем элемент ломаной линии как тег SVG
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    bool first = true;
    for (const auto& point : points_) {
        if (!first) {
            out << " ";
        }
        first = false;
        out << point.x << "," << point.y;
    }
    out << "\"";
    RenderAttrs(out); // Отображение дополнительных атрибутов, таких как заливка, обводка и т. д.
    out << " />"sv;
}

// Устанавливаем положение текстового элемента
Text& Text::SetPosition(Point pos) {
    position_ = pos;
    return *this;
}

// Устанавливаем смещение текстового элемента
Text& Text::SetOffset(Point offset) {
    offset_ = offset;
    return *this;
}

// Установить размер шрифта текста
Text& Text::SetFontSize(uint32_t size) {
    font_size_ = size;
    return *this;
}

// Установить семейство шрифтов текста
Text& Text::SetFontFamily(std::string font_family) {
    font_family_ = std::move(font_family);
    return *this;
}

// Устанавливаем толщину шрифта текста
Text& Text::SetFontWeight(std::string font_weight) {
    font_weight_ = std::move(font_weight);
    return *this;
}

// Устанавливаем данные (текстовое содержимое) текстового элемента
Text& Text::SetData(std::string data) {
    data_ = std::move(data);
    return *this;
}

// Отобразить текстовый элемент как тег SVG
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<text x=\""sv << position_.x << "\" y=\""sv << position_.y << "\""sv;
    out << " dx=\""sv << offset_.x << "\" dy=\""sv << offset_.y << "\""sv;
    out << " font-size=\""sv << font_size_ << "\""sv;
    if (!font_family_.empty()) {
        out << " font-family=\""sv << font_family_ << "\""sv;
    }
    if (!font_weight_.empty()) {
        out << " font-weight=\""sv << font_weight_ << "\""sv;
    }
    RenderAttrs(out); // Отображение дополнительных атрибутов, таких как заливка, обводка и т. д.
    out << ">"sv;
    // Экранирование специальных символов в текстовом содержимом
    for (char c : data_) {
        switch (c) {
            case '<': out << "&lt;"sv; break;
            case '>': out << "&gt;"sv; break;
            case '"': out << "&quot;"sv; break;
            case '&': out << "&amp;"sv; break;
            case '\'': out << "&apos;"sv; break;
            default: out << c;
        }
    }
    out << "</text>"sv;
}

// Добавить объект в документ
void Document::AddPtr(std::unique_ptr<Object>&& obj) {
    objects_.emplace_back(std::move(obj));
}

// Отобразить весь документ SVG
void Document::Render(std::ostream& out) const {
    out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
    out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;
    RenderContext context(out, 2, 2);
    for (const auto& obj : objects_) {
        obj->Render(context);
    }
    out << "</svg>"sv;
}

}  // namespace svg