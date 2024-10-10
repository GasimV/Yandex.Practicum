#include <iostream>
#include <list>
#include <iterator>
#include <string>

using namespace std;

class Editor {
private:
    list<char> text; // Текстовый буфер
    list<char> buffer; // Буфер обмена
    list<char>::iterator cursor; // Позиция курсора

    // Функция для перемещения курсора на указанное количество позиций
    list<char>::iterator AdvanceCursor(list<char>::iterator it, size_t tokens) {
        while (tokens > 0 && it != text.end()) {
            ++it;
            --tokens;
        }
        return it;
    }
public:
    Editor() : cursor(text.begin()) {}

    // Сдвиг курсора влево
    void Left() {
        if (cursor != text.begin()) {
            --cursor;
        }
    }

    // Сдвиг курсора вправо
    void Right() {
        if (cursor != text.end()) {
            ++cursor;
        }
    }

    // Вставка символа в текущую позицию курсора
    void Insert(char token) {
        text.insert(cursor, token);
    }

    // Вырезание не более tokens символов, начиная с текущей позиции курсора
    void Cut(size_t tokens = 1) {
        buffer.clear();
        auto end_cut = AdvanceCursor(cursor, tokens);
        buffer.splice(buffer.begin(), text, cursor, end_cut);
        cursor = end_cut;
    }

    // Копирование не более tokens символов, начиная с текущей позиции курсора
    void Copy(size_t tokens = 1) {
        buffer.clear();
        auto end_copy = AdvanceCursor(cursor, tokens);
        buffer.assign(cursor, end_copy);
    }

    // Вставка содержимого буфера в текущую позицию курсора
    void Paste() {
        if (!buffer.empty()) {
            text.insert(cursor, buffer.begin(), buffer.end());
        }
    }

    // Получение текущего содержимого текстового редактора
    string GetText() const {
        return string(text.begin(), text.end());
    }
};

int main() {
    Editor editor;
    const string text = "hello, world"s;
    for (char c : text) {
        editor.Insert(c);
    }
    // Текущее состояние редактора: `hello, world|`
    for (size_t i = 0; i < text.size(); ++i) {
        editor.Left();
    }
    // Текущее состояние редактора: `|hello, world`
    editor.Cut(7);
    // Текущее состояние редактора: `|world`
    // в буфере обмена находится текст `hello, `
    for (size_t i = 0; i < 5; ++i) {
        editor.Right();
    }
    // Текущее состояние редактора: `world|`
    editor.Insert(',');
    editor.Insert(' ');
    // Текущее состояние редактора: `world, |`
    editor.Paste();
    // Текущее состояние редактора: `world, hello, |`
    editor.Left();
    editor.Left();
    //Текущее состояние редактора: `world, hello|, `
    editor.Cut(3);  // Будут вырезаны 2 символа
    // Текущее состояние редактора: `world, hello|`
    cout << editor.GetText();
    return 0;
}