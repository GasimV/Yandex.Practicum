#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

const int MAX_USERS = 100000;
const int MAX_PAGES = 1000;

vector<int> user_progress(MAX_USERS + 1, 0);
vector<int> page_count(MAX_PAGES + 1, 0);

void process_read(int user, int page) {
    if (user_progress[user] > 0) {
        page_count[user_progress[user]]--;
    }
    user_progress[user] = page;
    page_count[page]++;
}

double process_cheer(int user) {
    if (user_progress[user] == 0) {
        return 0.0;
    }
    int user_page = user_progress[user];
    int readers_ahead = 0, total_readers = 0;
    
    for (int i = 1; i <= MAX_PAGES; ++i) {
        total_readers += page_count[i];
        if (i < user_page) {
            readers_ahead += page_count[i];
        }
    }
    if (total_readers <= 1) {
        return 1.0;
    }
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