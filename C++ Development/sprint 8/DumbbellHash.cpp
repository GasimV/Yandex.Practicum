#include <iostream>
#include <string>

using namespace std;

struct Circle {
    double x;
    double y;
    double r;
};

struct Dumbbell {
    Circle circle1;
    Circle circle2;
    string text;
};

struct CircleHasher {
    size_t operator() (const Circle& circle) const {
        size_t h_x = d_hasher_(circle.x);
        size_t h_y = d_hasher_(circle.y);
        size_t h_r = d_hasher_(circle.r);
        
        return h_x + h_y * 37 + h_r * (37 * 37);
    }

private:
    std::hash<double> d_hasher_;
};

struct DumbbellHash {
    size_t operator() (const Dumbbell& dumbbell) const {
        // Получаем хеши компонентов
        size_t h_circle1 = circle_hasher_(dumbbell.circle1);
        size_t h_circle2 = circle_hasher_(dumbbell.circle2);
        size_t h_text = text_hasher_(dumbbell.text);

        // Комбинируем хеши, используя различные степени 37
        return h_circle1 * (37 * 37 * 37 * 37) + // 37^4 для первой окружности
               h_circle2 * 37 +                  // 37^1 для второй окружности
               h_text;                           // 37^0 для текста
    }

private:
    CircleHasher circle_hasher_;
    std::hash<string> text_hasher_;
};

int main() {
    DumbbellHash hash;
    Dumbbell dumbbell{{10, 11.5, 2.3}, {3.14, 15, -8}, "abc"s};
    cout << "Dumbbell hash "s << hash(dumbbell);
}