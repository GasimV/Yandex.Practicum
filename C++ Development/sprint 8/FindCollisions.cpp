#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

template <typename Hash>
int FindCollisions(const Hash& hasher, istream& text) {
    unordered_map<size_t, unordered_set<string>> hash_map;
    int collisions = 0;
    string word;

    while (text >> word) {
        size_t hash_value = hasher(word);

        // Если хеш уже существует и слово не найдено в множестве для этого хеша, это коллизия
        if (hash_map[hash_value].count(word) == 0) {
            if (!hash_map[hash_value].empty()) {
                ++collisions;
            }
        }
        hash_map[hash_value].insert(move(word));
    }
    return collisions;
}

// Это плохой хешер. Его можно использовать для тестирования.
struct HasherDummy {
    size_t operator() (const string& str) const {
        size_t res = 0;
        for (char c : str) {
            res += static_cast<size_t>(c);
        }
        return res;
    }
};

int main() {
    hash<string> str_hasher;
    int collisions = FindCollisions(str_hasher, cin);
    cout << "Found collisions: "s << collisions << endl;
}