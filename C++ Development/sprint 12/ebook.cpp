#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

const int MAX_USERS = 100000;  // Максимальное количество пользователей
const int MAX_PAGES = 1000;    // Максимальное количество страниц в книге

// Вектор для хранения прогресса каждого пользователя по страницам
vector<int> user_progress(MAX_USERS + 1, 0);

// Вектор для хранения количества пользователей, дочитавших как минимум до страницы m
vector<int> page_count(MAX_PAGES + 1, 0);

// Функция обработки команды READ
void process_read(int user, int page) {
    // Если пользователь уже дочитал до определенной страницы, уменьшаем счетчик
    if (user_progress[user] > 0) {
        page_count[user_progress[user]]--;
    }
    // Обновляем прогресс пользователя и увеличиваем счетчик для новой страницы
    user_progress[user] = page;
    page_count[page]++;
}

// Функция обработки команды CHEER
// Возвращает долю пользователей, прочитавших меньше страниц, чем данный пользователь
double process_cheer(int user) {
    // Если пользователь еще не читал книгу, возвращаем 0.0
    if (user_progress[user] == 0) {
        return 0.0;
    }
    int user_page = user_progress[user];
    int readers_ahead = 0, total_readers = 0;
    
    // Подсчитываем общее количество читателей и тех, кто прочитал меньше страниц
    for (int i = 1; i <= MAX_PAGES; ++i) {
        total_readers += page_count[i];
        if (i < user_page) {
            readers_ahead += page_count[i];
        }
    }
    // Если пользователь единственный, возвращаем 1.0
    if (total_readers <= 1) {
        return 1.0;
    }
    // Вычисляем долю читателей, которые прочитали меньше
    return static_cast<double>(readers_ahead) / (total_readers - 1);
}

int main() {
    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; query_id++) {
        string query_type;
        cin >> query_type;
        int user_id;
        cin >> user_id;

        if (query_type == "READ") {
            int page_count;
            cin >> page_count;
            process_read(user_id, page_count);
        } else if (query_type == "CHEER") {
            cout << setprecision(6) << process_cheer(user_id) << '\n';
        }
    }
    return 0;
}