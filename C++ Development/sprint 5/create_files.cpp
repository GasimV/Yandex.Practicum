#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>

using namespace std;

// реализуйте эту функцию:
void CreateFiles(const string& config_file) {
    ifstream input(config_file);  // Открываем конфигурационный файл для чтения
    ofstream output;              // Поток для записи в создаваемые файлы
    string line;                  // Переменная для хранения строк из конфигурационного файла

    while (getline(input, line)) {
        if (!line.empty() && line[0] == '>') {
            // Если строка начинается с '>', удаляем его и записываем остальную часть в файл
            output << line.substr(1) << endl;
        } else {
            // Если новая строка не содержит '>', это имя нового файла
            if (output.is_open()) {
                output.close();  // Закрываем предыдущий файл, если он был открыт
            }
            output.open(line);  // Открываем новый файл для записи
        }
    }

    if (output.is_open()) {
        output.close();  // Закрываем последний файл, если он был открыт
    }
}

string GetLine(istream& in) {
    string s;
    getline(in, s);
    return s;
}

int main() {
    ofstream("test_config.txt"s) << "a.txt\n"
                                    ">10\n"
                                    ">abc\n"
                                    "b.txt\n"
                                    ">123"s;

    CreateFiles("test_config.txt"s);
    ifstream in_a("a.txt"s);
    assert(GetLine(in_a) == "10"s && GetLine(in_a) == "abc"s);

    ifstream in_b("b.txt"s);
    assert(GetLine(in_b) == "123"s);
}