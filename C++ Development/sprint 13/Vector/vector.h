#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <algorithm>
#include <stdexcept>

// Класс RawMemory отвечает только за выделение/освобождение памяти
template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

// Класс Vector теперь использует RawMemory для управления выделенной памятью.
// Конструкторы, деструктор и метод Reserve упрощены за счёт RawMemory.
template <typename T>
class Vector {
public:
    // Конструктор по умолчанию
    Vector() noexcept = default;

    // Конструктор с размером
    explicit Vector(size_t size)
        : size_(size)
        , data_(size) {
        size_t i = 0;
        try {
            for (; i < size_; ++i) {
                new (data_ + i) T();
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                (data_ + j)->~T();
            }
            throw;
        }
    }

    // Копирующий конструктор (аналогичен конструктору с размером)
    Vector(const Vector& other)
        : size_(other.size_)
        , data_(other.size_) {
        size_t i = 0;
        try {
            for (; i < size_; ++i) {
                new (data_ + i) T(other[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                (data_ + j)->~T();
            }
            throw;
        }
    }

    // Деструктор: вызываем деструкторы объектов, затем RawMemory сам освободит память
    ~Vector() {
        for (size_t i = 0; i < size_; ++i) {
            (data_ + i)->~T();
        }
    }

    // Метод резервирования памяти
    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        size_t i = 0;
        try {
            for (; i < size_; ++i) {
                new (new_data + i) T(data_[i]);
            }
        } catch (...) {
            for (size_t j = 0; j < i; ++j) {
                (new_data + j)->~T();
            }
            throw;
        }
        // Освобождаем старые объекты (память освободится в деструкторе RawMemory)
        for (size_t i = 0; i < size_; ++i) {
            (data_ + i)->~T();
        }
        data_.Swap(new_data);
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

    const T& operator[](size_t index) const noexcept {
        return data_[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

private:
    size_t size_ = 0;
    RawMemory<T> data_;
};