#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>        // для uninitialized_*/destroy_*
#include <type_traits>   // для is_nothrow_move_constructible_v, is_copy_constructible_v, is_pointer_v

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
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

// Класс Vector использует RawMemory и функции из <memory> для работы с неинициализированной памятью.
template <typename T>
class Vector {
public:
    // Конструктор по умолчанию
    Vector() noexcept = default;

    // Конструктор с размером: создаёт вектор заданного размера,
    // используя std::uninitialized_value_construct_n.
    explicit Vector(size_t size)
        : size_(size)
        , data_(size) {
        T* begin = data_.GetAddress();
        T* end = begin;
        try {
            end = std::uninitialized_value_construct_n(begin, size);
        } catch (...) {
            std::destroy(begin, end);
            throw;
        }
    }

    // Копирующий конструктор: копирует элементы с помощью std::uninitialized_copy_n.
    Vector(const Vector& other)
        : size_(other.size_)
        , data_(other.size_) {
        T* begin = data_.GetAddress();
        T* end = begin;
        try {
            auto copy_result = std::uninitialized_copy_n(other.data_.GetAddress(), size_, begin);
            // Если возвращается указатель, то используем его, иначе – извлекаем поле second.
            auto get_end = [](auto res) -> T* {
                if constexpr (std::is_pointer_v<decltype(res)>) {
                    return res;
                } else {
                    return res.second;
                }
            };
            end = get_end(copy_result);
        } catch (...) {
            std::destroy(begin, end);
            throw;
        }
    }

    // Деструктор: уничтожает элементы с помощью std::destroy_n.
    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

    // Метод Reserve: резервирует новую память и перемещает или копирует элементы.
    // Если тип T перемещается noexcept или не копируемый – используется перемещение.
    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        T* new_begin = new_data.GetAddress();
        T* new_end = new_begin;
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            auto move_result = std::uninitialized_move_n(data_.GetAddress(), size_, new_begin);
            auto get_end = [](auto res) -> T* {
                if constexpr (std::is_pointer_v<decltype(res)>) {
                    return res;
                } else {
                    return res.second;
                }
            };
            new_end = get_end(move_result);
        } else {
            auto copy_result = std::uninitialized_copy_n(data_.GetAddress(), size_, new_begin);
            auto get_end = [](auto res) -> T* {
                if constexpr (std::is_pointer_v<decltype(res)>) {
                    return res;
                } else {
                    return res.second;
                }
            };
            new_end = get_end(copy_result);
        }
        std::destroy_n(data_.GetAddress(), size_);
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