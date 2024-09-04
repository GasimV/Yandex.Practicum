#pragma once

#include <vector>
#include <iterator>
#include <iostream>
#include <cassert>  // Include assert for using assert()

template <typename Iterator>
class IteratorRange {
private:
    Iterator range_begin;
    Iterator range_end;
    size_t range_size;

public:
    IteratorRange(Iterator begin, Iterator end)
        : range_begin(begin), range_end(end), range_size(std::distance(begin, end)) {}

    Iterator begin() const { return range_begin; }
    Iterator end() const { return range_end; }
    size_t size() const { return range_size; }
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
        assert(page_size > 0);  // Ensure page size is positive

        // Check if range_begin equals range_end (empty container case)
        if (range_begin == range_end) {
            return;  // No pages to be created if the range is empty
        }

        for (Iterator it = range_begin; it != range_end;) {
            size_t current_page_size = std::min(page_size, static_cast<size_t>(std::distance(it, range_end)));
            Iterator next_it = std::next(it, current_page_size);  // Advance safely by page size
            pages.push_back({it, next_it});
            it = next_it;
        }
    }

    typename std::vector<IteratorRange<Iterator>>::const_iterator begin() const { return pages.begin(); }
    typename std::vector<IteratorRange<Iterator>>::const_iterator end() const { return pages.end(); }
    size_t size() const { return pages.size(); }
};

template <typename Container>
Paginator<typename Container::const_iterator> Paginate(const Container& c, size_t page_size) {
    return Paginator(c.begin(), c.end(), page_size);
}