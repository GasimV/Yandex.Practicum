#include <cassert>
#include <iostream>
#include <string_view>
#include <vector>
#include <algorithm>

using namespace std;

vector<string_view> SplitIntoWordsView(string_view str) {
    vector<string_view> result;

    // Убираем начальные пробелы
    str.remove_prefix(min(str.size(), str.find_first_not_of(' ')));

    while (!str.empty()) {
        // Находим позицию первого пробела
        size_t space = str.find(' ');

        // Добавляем вектор среза до первого пробела или до конца строки
        result.push_back(str.substr(0, space));

        // Сдвигаем строку на количество символов до первого пробела
        str.remove_prefix(min(str.size(), space));

        // Убираем возможные пробелы после слова
        str.remove_prefix(min(str.size(), str.find_first_not_of(' ')));
    }
    

    return result;
}

int main() {
    assert((SplitIntoWordsView("") == vector<string_view>{}));
    assert((SplitIntoWordsView("     ") == vector<string_view>{}));
    assert((SplitIntoWordsView("aaaaaaa") == vector{"aaaaaaa"sv}));
    assert((SplitIntoWordsView("a") == vector{"a"sv}));
    assert((SplitIntoWordsView("a b c") == vector{"a"sv, "b"sv, "c"sv}));
    assert((SplitIntoWordsView("a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    assert((SplitIntoWordsView("  a    bbb   cc   ") == vector{"a"sv, "bbb"sv, "cc"sv}));
    cout << "All OK" << endl;
}