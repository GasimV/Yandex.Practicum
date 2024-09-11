#include "log_duration.h"

#include <iostream>

using namespace std;

class StreamUntier {
public:
    // Конструктор отвязывает поток и сохраняет предыдущую привязку
    StreamUntier(istream& stream) : stream_(stream) {
        tied_before_ = stream_.tie(nullptr);  // Отвязываем поток, сохраняем предыдущую привязку
    }

    // Деструктор восстанавливает привязку
    ~StreamUntier() {
        stream_.tie(tied_before_);  // Восстанавливаем привязку
    }

private:
    istream& stream_;    // Ссылка на поток ввода (например, cin)
    ostream* tied_before_;  // Указатель на предыдущий связанный поток
};

int main() {
    LOG_DURATION("\\n with tie"s);

    StreamUntier guard(cin);
    int i;
    while (cin >> i) {
        cout << i * i << "\n"s;
    }

    return 0;
}