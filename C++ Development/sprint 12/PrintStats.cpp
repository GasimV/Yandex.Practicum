#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class Gender { FEMALE, MALE };

struct Person {
    int age;           // возраст
    Gender gender;     // пол
    bool is_employed;  // имеет ли работу
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
    if (range_begin == range_end) {
        return 0;
    }
    vector<typename InputIt::value_type> range_copy(range_begin, range_end);
    auto middle = range_copy.begin() + range_copy.size() / 2;
    nth_element(range_copy.begin(), middle, range_copy.end(), [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
    });
    return middle->age;
}

// напишите сигнатуру и реализацию функции PrintStats
void PrintStats(const vector<Person>& persons) {
    vector<Person> people = persons; // Создаем копию вектора для работы с partition

    cout << "Median age = " << ComputeMedianAge(people.begin(), people.end()) << endl;

    auto females_end = partition(people.begin(), people.end(), [](const Person& p) {
        return p.gender == Gender::FEMALE;
    });

    cout << "Median age for females = "s << ComputeMedianAge(people.begin(), females_end) << endl;
    cout << "Median age for males = "s << ComputeMedianAge(females_end, people.end()) << endl;

    auto employed_females_end = partition(people.begin(), females_end, [](const Person& p) {
        return p.is_employed;
    });

    cout << "Median age for employed females = "s << ComputeMedianAge(people.begin(), employed_females_end) << endl;
    cout << "Median age for unemployed females = "s << ComputeMedianAge(employed_females_end, females_end) << endl;

    auto employed_males_end = partition(females_end, people.end(), [](const Person& p) {
        return p.is_employed;
    });

    cout << "Median age for employed males = " << ComputeMedianAge(females_end, employed_males_end) << endl;
    cout << "Median age for unemployed males = "s << ComputeMedianAge(employed_males_end, people.end()) << endl;
}

int main() {
    vector<Person> persons = {
        {31, Gender::MALE, false},   {40, Gender::FEMALE, true},  {24, Gender::MALE, true},
        {20, Gender::FEMALE, true},  {80, Gender::FEMALE, false}, {78, Gender::MALE, false},
        {10, Gender::FEMALE, false}, {55, Gender::MALE, true},
    };
    PrintStats(persons);
}