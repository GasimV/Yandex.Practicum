#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

using namespace std;

// Определяет, будет ли слово палиндромом
// text может быть строкой, содержащей строчные буквы английского алфавита и пробелы
// Пустые строки и строки, состоящие только из пробелов, - это не палиндромы
bool IsPalindrome(string text) {
    // Шаг 1: Удалить начальные и конечные пробелы из входной строки
    
    // 1.1. Удалить начальные пробелы
    text.erase(text.begin(), find_if(text.begin(), text.end(), [](unsigned char ch) {
        return !isspace(ch);
    }));
    
    // 1.2. Удалить конечные пробелы
    text.erase(find_if(text.rbegin(), text.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base(), text.end());
    
    // Шаг 2: Проверка, является ли обрезанная строка пустой.
    if (text.empty()) {
        return false;
    }
    
    // Шаг 3: Нормализуем строку, удалив все пробелы
    text.erase(remove_if(text.begin(), text.end(), [](unsigned char ch) {
        return isspace(ch);
    }), text.end());
    
    // Шаг 4: Сравним строку с её обратной версией.
    string reversedText = text;
    reverse(reversedText.begin(), reversedText.end());
    
    // Шаг 5: Вернём true, если строка совпадает с перевёрнутой строкой
    return text == reversedText;
}

int main() {
    string text;
    getline(cin, text);

    if (IsPalindrome(text)) {
        cout << "palindrome"s << endl;
    } else {
        cout << "not a palindrome"s << endl;
    }
}