#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <memory>
#include <type_traits>

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
            auto get_end = [](auto res) -> T* {
                if constexpr (std::is_pointer_v<decltype(res)>)
                    return res;
                else
                    return res.second;
            };
            end = get_end(copy_result);
        } catch (...) {
            std::destroy(begin, end);
            throw;
        }
    }

    // Перемещающий конструктор: выполняется за O(1) и не выбрасывает исключений.
    Vector(Vector&& other) noexcept
        : size_(other.size_)
        , data_() // создаём пустой RawMemory
    {
        data_.Swap(other.data_);
        other.size_ = 0;
    }

    // Деструктор: уничтожает элементы с помощью std::destroy_n.
    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

    // Оператор копирующего присваивания.
    // Если вместимости вектора-приёмника не хватает для rhs,
    // применяется copy-and-swap, иначе – поэлементное копирование.
    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (data_.Capacity() < rhs.size_) {
                Vector<T> tmp(rhs);
                Swap(tmp);
            } else {
                size_t common = (size_ < rhs.size_) ? size_ : rhs.size_;
                for (size_t i = 0; i < common; ++i) {
                    data_[i] = rhs.data_[i];
                }
                if (rhs.size_ > size_) {
                    T* dest = data_.GetAddress() + size_;
                    size_t count = rhs.size_ - size_;
                    std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, count, dest);
                } else if (rhs.size_ < size_) {
                    std::destroy_n(data_.GetAddress() + rhs.size_, size_ - rhs.size_);
                }
                size_ = rhs.size_;
            }
        }
        return *this;
    }

    // Оператор перемещающего присваивания: выполняется за O(1) и не выбрасывает исключений.
    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        }
        return *this;
    }

    // Метод Swap: меняет содержимое векторов за O(1) и не выбрасывает исключений.
    void Swap(Vector& other) noexcept {
        std::swap(size_, other.size_);
        data_.Swap(other.data_);
    }

    // Метод Reserve: резервирует новую память и перемещает или копирует элементы.
    // Если тип T перемещается noexcept или не копируемый – используется перемещение.
    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        T* new_begin = new_data.GetAddress();
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            auto move_result = std::uninitialized_move_n(data_.GetAddress(), size_, new_begin);
            auto get_end = [](auto res) -> T* {
                if constexpr (std::is_pointer_v<decltype(res)>)
                    return res;
                else
                    return res.second;
            };
            (void)get_end(move_result);
        } else {
            auto copy_result = std::uninitialized_copy_n(data_.GetAddress(), size_, new_begin);
            auto get_end = [](auto res) -> T* {
                if constexpr (std::is_pointer_v<decltype(res)>)
                    return res;
                else
                    return res.second;
            };
            (void)get_end(copy_result);
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }

    // Метод Resize: изменяет размер вектора.
    // Если новый размер больше, добавляются элементы по умолчанию,
    // если меньше – лишние элементы уничтожаются.
    void Resize(size_t new_size) {
        if (new_size < size_) {
            std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
            size_ = new_size;
        } else if (new_size > size_) {
            if (new_size > Capacity()) {
                Reserve(new_size);
            }
            T* begin = data_.GetAddress() + size_;
            size_t count = new_size - size_;
            try {
                std::uninitialized_value_construct_n(begin, count);
            } catch (...) {
                std::destroy(begin, begin + count);
                throw;
            }
            size_ = new_size;
        }
    }

    // Вспомогательный шаблон для реализации PushBack.
    // Реализация двух версий PushBack делегирует работу этому шаблону.
    template<typename U>
    void PushBackImpl(U&& value) {
        if (size_ == Capacity()) {
            size_t new_capacity = (Capacity() == 0 ? 1 : Capacity() * 2);
            RawMemory<T> new_data(new_capacity);
            T* new_begin = new_data.GetAddress();
            // Перенос или копирование существующих элементов в новый буфер
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_begin);
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_begin);
            }
            T* new_elem_ptr = new_begin + size_;
            try {
                new (new_elem_ptr) T(std::forward<U>(value));
            } catch(...) {
                std::destroy_n(new_begin, size_);
                throw;  // Никаких изменений состояния вектора!
            }
            // Если вставка успешна, уничтожаем старые элементы и обновляем буфер
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
            ++size_;
        } else {
            new (data_.GetAddress() + size_) T(std::forward<U>(value));
            ++size_;
        }
    }

    // Метод PushBack (константная версия).
    // Если переданный элемент находится в памяти вектора, сначала копируется во временный объект.
    void PushBack(const T& value) {
        if (data_.GetAddress() && (data_.GetAddress() <= &value && &value < data_.GetAddress() + size_)) {
            T tmp = value;
            PushBackImpl(tmp);
        } else {
            PushBackImpl(value);
        }
    }

    // Метод PushBack (перемещающий).
    // Если переданный объект находится в памяти вектора, сначала перемещается во временный объект.
    void PushBack(T&& value) {
        if (data_.GetAddress() && (data_.GetAddress() <= &value && &value < data_.GetAddress() + size_)) {
            T tmp = std::move(value);
            PushBackImpl(std::move(tmp));
        } else {
            PushBackImpl(std::move(value));
        }
    }

    // Метод PopBack: удаляет последний элемент вектора.
    // Предполагается, что вектор не пуст; сложность O(1), noexcept.
    void PopBack() noexcept {
        assert(size_ > 0);
        --size_;
        (data_.GetAddress() + size_)->~T();
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

    template <typename... Args>
    T& EmplaceBack(Args&&... args) {
        if (size_ == Capacity()) {
            size_t new_capacity = (Capacity() == 0 ? 1 : Capacity() * 2);
            RawMemory<T> new_data(new_capacity);
            T* new_begin = new_data.GetAddress();

            // Перенос или копирование уже сконструированных элементов
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_begin);
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_begin);
            }
            
            T* new_elem_ptr = new_begin + size_;
            // Попытка сконструировать новый элемент с помощью переданных аргументов
            try {
                new (new_elem_ptr) T(std::forward<Args>(args)...);
            } catch (...) {
                // Если конструктор выбросил исключение, разрушить уже перенесённые элементы
                std::destroy_n(new_begin, size_);
                throw;
            }
            // Если вставка прошла успешно, уничтожаем старые элементы
            std::destroy_n(data_.GetAddress(), size_);
            // Меняем внутренний буфер на новый
            data_.Swap(new_data);
            ++size_;
            return data_[size_ - 1];
        } else {
            T* target = data_.GetAddress() + size_;
            new (target) T(std::forward<Args>(args)...);
            ++size_;
            return data_[size_ - 1];
        }
    }

    using iterator = T*;
    using const_iterator = const T*;

    iterator begin() noexcept {
        return data_.GetAddress();
    }
    iterator end() noexcept {
        return data_.GetAddress() + size_;
    }
    const_iterator begin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator end() const noexcept {
        return data_.GetAddress() + size_;
    }
    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator cend() const noexcept {
        return data_.GetAddress() + size_;
    }

    // Метод Emplace: вставляет элемент по указанной позиции с использованием perfect forwarding.
    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... args) {
        // Вычисляем индекс позиции вставки.
        size_t index = pos - data_.GetAddress();
        assert(index <= size_ && "Invalid iterator");

        // Если вставка в конец, делегируем EmplaceBack.
        if (index == size_) {
            return &EmplaceBack(std::forward<Args>(args)...);
        }

        // *** Вставка без реаллокации ***
        if (size_ < Capacity()) {
            T* pos_ptr = data_.GetAddress() + index;
            // Сначала создаём временный объект из переданных аргументов.
            T tmp(std::forward<Args>(args)...);
            // Сдвигаем элементы: перемещаем последний элемент в новый неинициализированный слот.
            new (data_.GetAddress() + size_) T(std::move(data_[size_ - 1]));
            // Сдвигаем оставшиеся элементы вправо.
            for (size_t i = size_ - 1; i > index; --i) {
                data_[i] = std::move(data_[i - 1]);
            }
            // Теперь выполняем move‑присваивание временного объекта в позицию вставки.
            *pos_ptr = std::move(tmp);
            ++size_;
            return pos_ptr;
        } else {
            // *** Вставка с реаллокацией ***
            size_t new_capacity = (Capacity() == 0 ? 1 : Capacity() * 2);
            RawMemory<T> new_data(new_capacity);
            T* new_begin = new_data.GetAddress();
            size_t i = 0;
            try {
                // Перемещаем (или копируем, если перемещение не noexcept) элементы до позиции вставки.
                for (; i < index; ++i) {
                    new (new_begin + i) T(std::move_if_noexcept(data_[i]));
                }
                // Конструируем новый элемент в позиции index.
                new (new_begin + index) T(std::forward<Args>(args)...);
                // Перемещаем оставшиеся элементы.
                for (; i < size_; ++i) {
                    new (new_begin + i + 1) T(std::move_if_noexcept(data_[i]));
                }
            } catch (...) {
                // При исключении разрушаем уже сконструированные элементы.
                for (size_t j = 0; j < i; ++j) {
                    new_begin[j].~T();
                }
                throw;
            }
            // Разрушаем старые элементы и обновляем буфер.
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
            ++size_;
            return data_.GetAddress() + index;
        }
    }

    // Методы Insert делегируют работу методу Emplace.
    iterator Insert(const_iterator pos, const T& value) {
        // Если переданный объект находится в векторе, сначала создаём временную копию.
        if (data_.GetAddress() && (data_.GetAddress() <= &value && &value < data_.GetAddress() + size_)) {
            T tmp = value;
            return Emplace(pos, std::move(tmp));
        } else {
            return Emplace(pos, value);
        }
    }
    iterator Insert(const_iterator pos, T&& value) {
        // Если переданный объект находится в векторе, сначала перемещаем его во временный объект.
        if (data_.GetAddress() && (data_.GetAddress() <= &value && &value < data_.GetAddress() + size_)) {
            T tmp = std::move(value);
            return Emplace(pos, std::move(tmp));
        } else {
            return Emplace(pos, std::move(value));
        }
    }
    
    // Метод Erase: удаляет элемент по указанной позиции.
    iterator Erase(const_iterator pos) noexcept(noexcept(std::declval<T&>() = std::move(std::declval<T&>()))) {
        T* pos_ptr = const_cast<T*>(pos);
        std::move(pos_ptr + 1, data_.GetAddress() + size_, pos_ptr);
        PopBack();
        return pos_ptr;
    }

private:
    size_t size_ = 0;
    RawMemory<T> data_;
};