#pragma once

#include <cassert>
#include <initializer_list>
#include <stdexcept>
#include <algorithm>

template <typename Type>
class SimpleVector {
private:
    size_t size_ = 0;
    size_t capacity_ = 0;
    Type* data_ = nullptr;

public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : size_(size), capacity_(size), data_(size ? new Type[size]() : nullptr) {}

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) : size_(size), capacity_(size), data_(size ? new Type[size] : nullptr) {
        std::fill(begin(), end(), value);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init)
        : size_(init.size()), capacity_(init.size()), data_(size_ ? new Type[size_] : nullptr) {
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other)
        : size_(other.size_), capacity_(other.capacity_), data_(other.size_ ? new Type[other.capacity_] : nullptr) {
            std::copy(other.begin(), other.end(), begin());
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    // Деструктор
    ~SimpleVector() {
        delete[] data_;
    }

    void PushBack(const Type& item) {
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
            Type* new_data = new Type[new_capacity];
            std::copy(begin(), end(), new_data);
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        }
        data_[size_++] = item;
    }

    void PopBack() noexcept {
        if (size_ > 0) {
            --size_;
        }
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        size_t index = pos - begin();
        if (size_ == capacity_) {
            size_t new_capacity = capacity_ == 0 ? 1 : 2 * capacity_;
            Type* new_data = new Type[new_capacity];
            std::copy(begin(), begin() + index, new_data);
            new_data[index] = value;
            std::copy(begin() + index, end(), new_data + index + 1);
            delete[] data_;
            data_ = new_data;
            capacity_ = new_capacity;
        } else {
            std::copy_backward(begin() + index, end(), end() + 1);
            data_[index] = value;
        }
        ++size_;
        return begin() + index;
    }

    Iterator Erase(ConstIterator pos) {
        size_t index = pos - begin();
        std::copy(begin() + index + 1, end(), begin() + index);
        --size_;
        return begin() + index;
    }

    void swap(SimpleVector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return data_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (index >= size_) {
            throw std::out_of_range("Index out of range");
        }
        return data_[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        } else if (new_size <= capacity_) {
            std::fill(begin() + size_, begin() + new_size, Type());
            size_ = new_size;
        } else {
            size_t new_capacity = std::max(new_size, 2 * capacity_);
            Type* new_data = new Type[new_capacity];
            std::copy(begin(), end(), new_data);
            std::fill(new_data + size_, new_data + new_size, Type());
            delete[] data_;
            data_ = new_data;
            size_ = new_size;
            capacity_ = new_capacity;
        }
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return data_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return data_ + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return data_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return data_ + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return data_;
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return data_ + size_;
    }
};

// Операторы сравнения
template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}