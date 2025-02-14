#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <algorithm>
#include <stdexcept>

template <typename T>
class Vector {
public:
    // Конструктор по умолчанию
    Vector() noexcept = default;

    // Конструктор с размером
    explicit Vector(size_t size)
        : size_(0), capacity_(size), data_(size ? static_cast<T*>(operator new(size * sizeof(T))) : nullptr) {
        try {
            for (; size_ < size; ++size_) {
                new (data_ + size_) T();
            }
        } catch (...) {
            Clear();
            operator delete(data_);
            throw;
        }
    }

    // Копирующий конструктор
    Vector(const Vector& other)
        : size_(0), capacity_(other.size_), data_(other.size_ ? static_cast<T*>(operator new(other.size_ * sizeof(T))) : nullptr) {
        try {
            for (; size_ < other.size_; ++size_) {
                new (data_ + size_) T(other.data_[size_]);
            }
        } catch (...) {
            Clear();
            operator delete(data_);
            throw;
        }
    }

    // Деструктор
    ~Vector() {
        Clear();
        operator delete(data_);
    }

    // Метод резервирования памяти
    void Reserve(size_t capacity) {
        if (capacity <= capacity_) {
            return;
        }
        
        T* new_data = nullptr;
        try {
            new_data = static_cast<T*>(operator new(capacity * sizeof(T)));
            size_t i = 0;
            try {
                for (; i < size_; ++i) {
                    new (new_data + i) T(data_[i]);
                }
            } catch (...) {
                for (size_t j = 0; j < i; ++j) {
                    new_data[j].~T();
                }
                operator delete(new_data);
                throw;
            }
        } catch (const std::bad_alloc&) {
            throw std::runtime_error("Memory allocation failed");
        }
        
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        operator delete(data_);
        data_ = new_data;
        capacity_ = capacity;
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return capacity_;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

private:
    void Clear() noexcept {
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    size_t size_ = 0;
    size_t capacity_ = 0;
    T* data_ = nullptr;
};
