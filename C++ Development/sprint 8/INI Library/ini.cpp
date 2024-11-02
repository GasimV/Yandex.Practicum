#include "ini.h"
#include <sstream>
#include <algorithm>
#include <cctype>

using namespace std;

// место для реализаций методов и функций библиотеки ini
// не забудьте, что они должны быть помещены в namespace ini
namespace ini {

Section& Document::AddSection(string name) {
    return sections_[move(name)];
}

const Section& Document::GetSection(const string& name) const {
    const static Section empty_section; // пустая секция для возвращения при отсутствии
    auto it = sections_.find(name);
    return it != sections_.end() ? it->second : empty_section;
}

size_t Document::GetSectionCount() const {
    return sections_.size();
}

string Trim(const string& str) {
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos || end == string::npos) ? "" : str.substr(start, end - start + 1);
}

Document Load(istream& input) {
    Document doc;
    Section* current_section = nullptr;

    string line;
    while (getline(input, line)) {
        // Удаляем начальные и конечные пробелы
        line = Trim(line);

        if (line.empty()) {
            continue; // Пропускаем пустые строки
        }

        // Обработка секции
        if (line.front() == '[' && line.back() == ']') {
            // Если строка определяет новую секцию
            string section_name = Trim(line.substr(1, line.size() - 2)); // Извлекаем имя секции без скобок
            current_section = &doc.AddSection(section_name);
        } else if (current_section) {
            // Парсим строку вида ключ=значение
            auto eq_pos = line.find('=');
            if (eq_pos != string::npos) {
                string key = Trim(line.substr(0, eq_pos));
                string value = Trim(line.substr(eq_pos + 1));

                (*current_section)[key] = value;
            }
        }
    }
    return doc;
}

} // namespace ini