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
    IteratorRange(Iterator begin, Iterator end);

    Iterator begin() const;
    Iterator end() const;
    size_t size() const;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const IteratorRange<Iterator>& range);

template <typename Iterator>
class Paginator {
private:
    std::vector<IteratorRange<Iterator>> pages;

public:
    Paginator(Iterator range_begin, Iterator range_end, size_t page_size);

    auto begin() const;
    auto end() const;
    size_t size() const;
};

template <typename Container>
auto Paginate(const Container& c, size_t page_size);