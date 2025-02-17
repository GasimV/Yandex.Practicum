#pragma once

#include <stdexcept>
#include <utility>
#include <new>

class BadOptionalAccess : public std::exception {
public:
    using exception::exception;

    virtual const char* what() const noexcept override {
        return "Bad optional access";
    }
};

template <typename T>
class Optional {
public:
    Optional() = default;
    Optional(const T& value) {
        new (data_) T(value);
        is_initialized_ = true;
    }
    Optional(T&& value) {
        new (data_) T(std::move(value));
        is_initialized_ = true;
    }
    Optional(const Optional& other) {
        if (other.is_initialized_) {
            new (data_) T(*other);
            is_initialized_ = true;
        }
    }
    Optional(Optional&& other) noexcept {
        if (other.is_initialized_) {
            new (data_) T(std::move(*other));
            is_initialized_ = true;
        }
    }
    
    Optional& operator=(const T& value) {
        if (is_initialized_) {
            **this = value;
        } else {
            new (data_) T(value);
            is_initialized_ = true;
        }
        return *this;
    }
    Optional& operator=(T&& value) {
        if (is_initialized_) {
            **this = std::move(value);
        } else {
            new (data_) T(std::move(value));
            is_initialized_ = true;
        }
        return *this;
    }
    Optional& operator=(const Optional& rhs) {
        if (this != &rhs) {
            if (rhs.is_initialized_) {
                *this = *rhs;
            } else {
                Reset();
            }
        }
        return *this;
    }
    Optional& operator=(Optional&& rhs) noexcept {
        if (this != &rhs) {
            if (rhs.is_initialized_) {
                *this = std::move(*rhs);
            } else {
                Reset();
            }
        }
        return *this;
    }
    
    ~Optional() {
        Reset();
    }

    bool HasValue() const {
        return is_initialized_;
    }

    T& operator*() {
        return *reinterpret_cast<T*>(data_);
    }
    const T& operator*() const {
        return *reinterpret_cast<const T*>(data_);
    }
    T* operator->() {
        return reinterpret_cast<T*>(data_);
    }
    const T* operator->() const {
        return reinterpret_cast<const T*>(data_);
    }

    T& Value() {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return **this;
    }
    const T& Value() const {
        if (!is_initialized_) {
            throw BadOptionalAccess();
        }
        return **this;
    }

    void Reset() {
        if (is_initialized_) {
            reinterpret_cast<T*>(data_)->~T();
            is_initialized_ = false;
        }
    }

    template <typename... Args>
    void Emplace(Args&&... args) {
        Reset(); // Уничтожаем предыдущее значение, если оно было
        new (data_) T(std::forward<Args>(args)...); // Конструируем новое значение на месте
        is_initialized_ = true;
    }

private:
    alignas(T) char data_[sizeof(T)];
    bool is_initialized_ = false;
};