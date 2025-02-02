#include <iostream>
#include <unordered_map>
#include <vector>
#include <random>
#include <string>

using namespace std;

class RandomContainer {
public:
    void Insert(int val) {
        if (index_map_.find(val) == index_map_.end()) {
            values_pool_.push_back(val);
            index_map_[val] = values_pool_.size() - 1;
        }
    }

    void Remove(int val) {
        if (index_map_.find(val) != index_map_.end()) {
            int index = index_map_[val];
            int last_val = values_pool_.back();
            values_pool_[index] = last_val;
            index_map_[last_val] = index;
            values_pool_.pop_back();
            index_map_.erase(val);
        }
    }

    bool Has(int val) const {
        return index_map_.find(val) != index_map_.end();
    }

    int GetRand() const {
        uniform_int_distribution<int> distr(0, values_pool_.size() - 1);
        int rand_index = distr(engine_);
        return values_pool_[rand_index];
    }

private:
    vector<int> values_pool_;
    unordered_map<int, int> index_map_;
    mutable mt19937 engine_{random_device{}()};
};

int main() {
    RandomContainer container;
    int query_num = 0;
    cin >> query_num;
    
    for (int query_id = 0; query_id < query_num; query_id++) {
        string query_type;
        cin >> query_type;
        
        if (query_type == "INSERT") {
            int value = 0;
            cin >> value;
            container.Insert(value);
        } else if (query_type == "REMOVE") {
            int value = 0;
            cin >> value;
            container.Remove(value);
        } else if (query_type == "HAS") {
            int value = 0;
            cin >> value;
            cout << (container.Has(value) ? "true" : "false") << endl;
        } else if (query_type == "RAND") {
            cout << container.GetRand() << endl;
        }
    }
    
    return 0;
}