#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

class MoneyBox {
public:
    explicit MoneyBox(vector<int64_t> nominals)
        : nominals_(move(nominals))
        , counts_(nominals_.size()) {
    }

    const vector<int>& GetCounts() const {
        return counts_;
    }

    void PushCoin(int64_t value) {
        int index = GetNominalIndex(value);
            if (index != -1) {
                ++counts_[index];
            }
    }

    void PrintCoins(ostream& out) const {
        for (size_t i = 0; i < nominals_.size(); ++i) {
            if (counts_[i] > 0) {
                out << nominals_[i] << ": " << counts_[i] << endl;
            }
        }
    }

private:
    const vector<int64_t> nominals_;
    vector<int> counts_;

    int GetNominalIndex(int64_t value) const {
        for (size_t i = 0; i < nominals_.size(); ++i) {
            if (nominals_[i] == value) {
                return i;
            }
        }
        return -1; // если номинал не найден
    }
};

ostream& operator<<(ostream& out, const MoneyBox& cash) {
    cash.PrintCoins(out);
    return out;
}

int main() {
    MoneyBox cash({10, 50, 100, 200, 500, 1000, 2000, 5000});

    int times;
    cout << "Enter number of coins you have:"s << endl;
    cin >> times;

    cout << "Enter all nominals:"s << endl;
    for (int i = 0; i < times; ++i) {
        int64_t value;
        cin >> value;
        cash.PushCoin(value);
    }

    cout << cash << endl;
}