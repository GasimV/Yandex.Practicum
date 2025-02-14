#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <algorithm>

template <typename T>
class Vector {
public:
    // Конструктор по умолчанию
    Vector() noexcept = default;

    // Конструктор с размером
    explicit Vector(size_t size)
        : size_(size),
          capacity_(size),
          data_(size ? static_cast<T*>(operator new(size * sizeof(T))) : nullptr) {
        for (size_t i = 0; i < size_; ++i) {
            new (data_ + i) T(); // Конструирование элементов
        }
    }

    // Копирующий конструктор
    Vector(const Vector& other)
        : size_(other.size_),
          capacity_(other.size_),
          data_(other.size_ ? static_cast<T*>(operator new(other.size_ * sizeof(T))) : nullptr) {
        for (size_t i = 0; i < size_; ++i) {
            new (data_ + i) T(other.data_[i]); // Копирование элементов
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
        
        T* new_data = static_cast<T*>(operator new(capacity * sizeof(T)));
        
        for (size_t i = 0; i < size_; ++i) {
            new (new_data + i) T(std::move(data_[i]));
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