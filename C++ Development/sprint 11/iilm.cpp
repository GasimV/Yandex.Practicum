#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <map>
#include <iterator>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end)
        : first(begin)
        , last(end) {
    }

    Iterator begin() const {
        return first;
    }

    Iterator end() const {
        return last;
    }

private:
    Iterator first, last;
};

template <typename Collection>
auto Head(const Collection& v, size_t top) {
    return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
    string name;
    int age, income;
    bool is_male;
};

vector<Person> ReadPeople(istream& input) {
    int count;
    input >> count;

    vector<Person> result(count);
    for (Person& p : result) {
        char gender;
        input >> p.name >> p.age >> p.income >> gender;
        p.is_male = (gender == 'M');
    }

    return result;
}

int main() {
    // Read input data
    const vector<Person> people = [&]() {
        auto data = ReadPeople(cin);
        sort(data.begin(), data.end(), [](const Person& lhs, const Person& rhs) {
            return lhs.age < rhs.age;
        });
        return data;
    }();

    // Pre-sort by income for WEALTHY command
    const vector<Person> income_sorted_people = [&]() {
        auto sorted = people;
        sort(sorted.begin(), sorted.end(), [](const Person& lhs, const Person& rhs) {
            return lhs.income > rhs.income;
        });
        return sorted;
    }();

    // Process commands
    for (string command; cin >> command;) {
        if (command == "AGE"s) {
            int adult_age;
            cin >> adult_age;

            auto adult_begin = lower_bound(people.begin(), people.end(), adult_age, 
            	[](const Person& lhs, int age) {
                return lhs.age < age;
            });

            cout << "There are "s << distance(adult_begin, people.end()) << " adult people for maturity age "s
                 << adult_age << '\n';
        } else if (command == "WEALTHY"s) {
            int count;
            cin >> count;

            const int total_income = accumulate(income_sorted_people.begin(), 
                income_sorted_people.begin() + min(count, static_cast<int>(income_sorted_people.size())), 
                0, [](int sum, const Person& p) {
                    return sum + p.income;
            });
            cout << "Top-"s << count << " people have total income "s << total_income << '\n';
        } else if (command == "POPULAR_NAME"s) {
            char gender;
            cin >> gender;

            const bool is_male = (gender == 'M');
            map<string, int> name_counts;

            for (const auto& person : people) {
                if (person.is_male == is_male) {
                    name_counts[person.name]++;
                }
            }

            if (name_counts.empty()) {
                cout << "No people of gender "s << gender << '\n';
            } else {
                const auto& most_popular_name = min_element(
                    name_counts.begin(), name_counts.end(),
                    [](const auto& lhs, const auto& rhs) {
                        return lhs.second > rhs.second || (lhs.second == rhs.second && lhs.first < rhs.first);
                    })->first;
                cout << "Most popular name among people of gender "s << gender << " is "s << most_popular_name << '\n';
            }
        }
    }
}