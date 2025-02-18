#include <iostream>
#include <map>
#include <set>
#include <string>
#include <cmath>

using namespace std;

class RouteManager {
public:
    void AddRoute(int start, int finish) {
        reachable_lists_[start].insert(finish);
        reachable_lists_[finish].insert(start);
    }
    
    int FindNearestFinish(int start, int finish) const {
        int result = abs(start - finish);
        if (reachable_lists_.count(start) == 0) {
            return result;
        }
        const set<int>& reachable_stations = reachable_lists_.at(start);

        auto lower = reachable_stations.lower_bound(finish);
        if (lower != reachable_stations.end()) {
            result = min(result, abs(finish - *lower));
        }
        if (lower != reachable_stations.begin()) {
            result = min(result, abs(finish - *prev(lower)));
        }

        return result;
    }

private:
    map<int, set<int>> reachable_lists_;
};

int main() {
    RouteManager routes;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;
        int start, finish;
        cin >> start >> finish;
        if (query_type == "ADD"s) {
            routes.AddRoute(start, finish);
        } else if (query_type == "GO"s) {
            cout << routes.FindNearestFinish(start, finish) << "\n"s;
        }
    }
}