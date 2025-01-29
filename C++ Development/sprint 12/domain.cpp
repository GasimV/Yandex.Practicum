#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <iterator>

using namespace std;

// Класс, представляющий домен
class Domain {
public:
    // Конструктор из строки с добавлением ведущей точки и разворотом строки
    explicit Domain(const std::string& domain_str) {
        std::string with_dot = "." + domain_str;
        reversed_ = with_dot;
        std::reverse(reversed_.begin(), reversed_.end());
    }

    // Оператор равенства
    bool operator==(const Domain& other) const {
        return reversed_ == other.reversed_;
    }

    // Метод проверки, является ли текущий домен поддоменом другого домена
    bool IsSubdomain(const Domain& other) const {
        if (reversed_.size() < other.reversed_.size()) {
            return false;
        }
        return std::equal(other.reversed_.begin(), other.reversed_.end(), reversed_.begin());
    }

    // Метод для получения перевёрнутого домена
    std::string GetReversed() const {
        return reversed_;
    }

private:
    std::string reversed_;
};

// Функция для чтения доменов из потока
std::vector<Domain> ReadDomains(std::istream& input, size_t count) {
    std::vector<Domain> domains;
    domains.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        std::string line;
        std::getline(input, line);
        if (!line.empty()) {
            domains.emplace_back(line);
        }
    }
    return domains;
}

// Класс для проверки доменов на запрещённость
class DomainChecker {
public:
    // Конструктор, принимающий итераторы на список запрещённых доменов
    template <typename Iterator>
    DomainChecker(Iterator begin, Iterator end) {
        forbidden_domains_.reserve(std::distance(begin, end));
        for (auto it = begin; it != end; ++it) {
            forbidden_domains_.emplace_back(it->GetReversed());
        }
        std::sort(forbidden_domains_.begin(), forbidden_domains_.end());

        // Удаляем избыточные поддомены
        auto new_end = std::unique(forbidden_domains_.begin(), forbidden_domains_.end(),
            [&](const std::string& a, const std::string& b) {
                return a == b || (a.size() < b.size() && std::equal(a.begin(), a.end(), b.begin()));
            });
        forbidden_domains_.erase(new_end, forbidden_domains_.end());
    }

    // Метод проверки, является ли домен запрещённым
    bool IsForbidden(const Domain& domain) const {
        const std::string& reversed = domain.GetReversed();
        // Используем upper_bound для поиска потенциального совпадения
        auto it = std::upper_bound(forbidden_domains_.begin(), forbidden_domains_.end(), reversed);
        if (it == forbidden_domains_.begin()) {
            return false;
        }
        --it;
        // Проверяем, является ли запрещённый домен префиксом текущего домена
        return IsPrefix(*it, reversed);
    }

private:
    std::vector<std::string> forbidden_domains_;

    // Проверка, является ли a префиксом b
    bool IsPrefix(const std::string& a, const std::string& b) const {
        if (a.size() > b.size()) return false;
        return std::equal(a.begin(), a.end(), b.begin());
    }
};

// Функция для чтения числа из потока
template <typename Number>
Number ReadNumberOnLine(std::istream& input) {
    std::string line;
    std::getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(cin, ReadNumberOnLine<size_t>(cin));
    for (const Domain& domain : test_domains) {
        std::cout << (checker.IsForbidden(domain) ? "Bad" : "Good") << std::endl;
    }

    return 0;
}