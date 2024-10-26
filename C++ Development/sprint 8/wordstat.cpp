#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <tuple>
#include <vector>
#include <fstream>

using namespace std;

vector<pair<string, int>> GetSortedWordCounts(vector<string> words) {
    map<string, int> counts_map;

    for (auto& word : words) {
        ++counts_map[move(word)];
    }

    vector<pair<string, int>> counts_vector(move_iterator(counts_map.begin()), move_iterator(counts_map.end()));
    sort(counts_vector.begin(), counts_vector.end(), [](const auto& l, const auto& r) {
        return l.second > r.second;
    });

    return counts_vector;
}

int main() {
    vector<string> words;
    string word;

    ifstream file("canterbury.txt");
    if (!file.is_open()) {
        cerr << "Error: Could not open the file." << endl;
        return 1;
    }

    while (file >> word) {
        words.push_back(word);
    }

    file.close();

    auto counts_vector = GetSortedWordCounts(move(words));
    
    for (const auto& [w, count] : counts_vector) {
        if (w.size() == 5) {
            cout << "Most frequent 5-letter word: " << w << "-" << count << " occurrences" << endl;
            break;
        }
    }
}