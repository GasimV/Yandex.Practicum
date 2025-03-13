#include "common.h"

#include <cctype>
#include <sstream>

const int LETTERS = 26;
const int MAX_POSITION_LENGTH = 17;
const int MAX_POS_LETTER_COUNT = 3;

const Position Position::NONE = {-1, -1};

// Проверяет, является ли позиция валидной
bool Position::IsValid() const {
    return row >= 0 && row < MAX_ROWS && col >= 0 && col < MAX_COLS;
}

// Преобразует позицию в строку формата "A1"
std::string Position::ToString() const {
    if (!IsValid()) return "";

    std::string col_str;
    int c = col;
    while (c >= 0) {
        col_str.insert(col_str.begin(), 'A' + (c % LETTERS));
        c = (c / LETTERS) - 1;
    }

    return col_str + std::to_string(row + 1);
}

// Создаёт позицию из строки формата "A1"
Position Position::FromString(std::string_view str) {
    if (str.empty() || !std::isalpha(str[0])) {
        return Position::NONE;
    }

    // Разделение на буквенную и числовую части
    int i = 0;
    int len = static_cast<int>(str.size());
    while (i < len && std::isalpha(str[i])) {
        i++;
    }

    if (i == 0 || i > MAX_POS_LETTER_COUNT || i == len) {
        return Position::NONE;
    }

    std::string col_str = std::string(str.substr(0, i));
    std::string row_str = std::string(str.substr(i));

    // Проверяем, что строка не пустая и состоит только из цифр
    if (row_str.empty() || row_str.find_first_not_of("0123456789") != std::string::npos) {
        return Position::NONE;
    }

    // Пробуем преобразовать строку в число
    int row;
    try {
        row = std::stoi(row_str) - 1;
    } catch (...) { // Перехватываем все исключения (stoi может выбросить std::invalid_argument или std::out_of_range)
        return Position::NONE;
    }

    // Проверяем границы строки
    if (row < 0 || row >= MAX_ROWS) {
        return Position::NONE;
    }

    // Конвертация колонки
    int col = 0;
    for (char ch : col_str) {
        if (!std::isupper(ch)) {
            return Position::NONE;
        }
        col = col * LETTERS + (ch - 'A' + 1);
        if (col > MAX_COLS) {  // Защита от переполнения
            return Position::NONE;
        }
    }
    col -= 1; // Приведение к 0-индексации

    // Проверка границ столбца
    if (col < 0 || col >= MAX_COLS) {
        return Position::NONE;
    }

    return Position{row, col};
}

// Оператор сравнения "=="
bool Position::operator==(const Position rhs) const {
    return row == rhs.row && col == rhs.col;
}

// Оператор сравнения "<" (по строке, затем по столбцу)
bool Position::operator<(const Position rhs) const {
    return (row < rhs.row) || (row == rhs.row && col < rhs.col);
}