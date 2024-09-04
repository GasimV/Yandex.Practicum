#pragma once

#include <vector>
#include <iterator>
#include <iostream>

template <typename Iterator>
class IteratorRange {
private:
    Iterator range_begin;
    Iterator range_end;

public:
    // Constructor to initialize the range
    IteratorRange(Iterator begin, Iterator end)
        : range_begin(begin), range_end(end) {}

    Iterator begin() const { return range_begin; }
    Iterator end() const { return range_end; }
    size_t size() const { return std::distance(range_begin, range_end); }
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const IteratorRange<Iterator>& range) {
    for (auto it = range.begin(); it != range.end(); ++it) {
        os << *it << ' ';
    }
    return os;
}

template <typename Iterator>
class Paginator {
private:
    std::vector<IteratorRange<Iterator>> pages;

public:
    Paginator(Iterator range_begin, Iterator range_end, size_t page_size) {
        for (Iterator current = range_begin; current != range_end;) {
            Iterator next = current;
            std::advance(next, std::min(page_size, static_cast<size_t>(std::distance(current, range_end))));
            pages.push_back({current, next});
            current = next;
        }
    }

    auto begin() const { return pages.begin(); }
    auto end() const { return pages.end(); }
    size_t size() const { return pages.size(); }
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}