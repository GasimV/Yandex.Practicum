#include <string_view>
#include <string>
#include <iostream>
#include <cassert>
#include <unordered_map>
#include <vector>

using namespace std;

class Translator {
public:
    // Добавление пары source-target
    void Add(string_view source, string_view target) {
        // Сохраняем строки, чтобы они существовали до уничтожения объекта Translator
        strings.push_back(string(source));
        strings.push_back(string(target));

        // Прямой перевод
        forward_map[strings[strings.size() - 2]] = strings.back();

        // Обратный перевод
        backward_map[strings.back()] = strings[strings.size() - 2];
    }
    
    // Прямой перевод (язык 1 -> язык 2)
    string_view TranslateForward(string_view source) const {
        auto it = forward_map.find(source);
        if (it != forward_map.end()) {
            return it->second;
        }
        return {}; // Возвращаем пустую строку, если перевод не найден
    }
    
    // Обратный перевод (язык 2 -> язык 1)
    string_view TranslateBackward(string_view target) const {
        auto it = backward_map.find(target);
        if (it != backward_map.end()) {
            return it->second;
        }
        return {}; // Возвращаем пустую строку, если перевод не найден
    }

private:
    // Контейнер для хранения строк
    vector<string> strings;

    // Словари для прямого и обратного перевода
    unordered_map<string_view, string_view> forward_map;
    unordered_map<string_view, string_view> backward_map;
};

void TestSimple() {
    Translator translator;
    translator.Add(string("okno"s), string("window"s));
    translator.Add(string("stol"s), string("table"s));

    // Assertion failed: translator.TranslateForward("okno"s) == "window"s
    // Reason: string_view does not own the strings it references, it only stores a pointer to the memory location 
    // where the string is. When you add new strings to strings (vector), it may cause a reallocation if 
    // the vector grows and requires more memory. 
    // Reallocation may move all the elements of the vector to a new memory location, which 
    // will make any existing string_views in dictionaries invalid (since they point to old memory that no longer exists).
    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);
}

// Test without fail!
void TestSimple2() {
    Translator translator;

    translator.Add(string("okno"s), string("window"s));
    assert(translator.TranslateForward("okno"s) == "window"s);
    assert(translator.TranslateBackward("window"s) == "okno"s);
    
    translator.Add(string("stol"s), string("table"s));
    assert(translator.TranslateBackward("table"s) == "stol"s);
    assert(translator.TranslateForward("table"s) == ""s);

    // The tests pass because at the time of checking the translation "window", 
    // the second pair "stol" -> "table" has not yet been added. 
    // That is, the memory reallocation has not yet occurred, and the string_view references remain valid 
    // until the new pair is added.
}

int main() {
    TestSimple();
    TestSimple2();
}