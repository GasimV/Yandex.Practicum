#pragma once

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <algorithm>
#include <stdexcept>

template <typename Type>
class SingleLinkedList {
    // Узел списка
    struct Node {
        Node() = default;
        Node(const Type& val, Node* next)
            : value(val), next_node(next) {
        }
        Type value;
        Node* next_node = nullptr;
    };

    // Шаблон класса BasicIterator
    template <typename ValueType>
    class BasicIterator {
        friend class SingleLinkedList;

        explicit BasicIterator(Node* node) : node_(node) {}

        explicit BasicIterator(const Node* node) : node_(const_cast<Node*>(node)) {}

    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Type;
        using difference_type = std::ptrdiff_t;
        using pointer = ValueType*;
        using reference = ValueType&;

        BasicIterator() = default;

        BasicIterator(const BasicIterator<Type>& other) noexcept : node_(other.node_) {}

        BasicIterator& operator=(const BasicIterator& rhs) = default;

        [[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        [[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
            return node_ == rhs.node_;
        }

        [[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
            return !(*this == rhs);
        }

        BasicIterator& operator++() noexcept {
            assert(node_ != nullptr);
            node_ = node_->next_node;
            return *this;
        }

        BasicIterator operator++(int) noexcept {
            BasicIterator temp = *this;
            ++(*this);
            return temp;
        }

        [[nodiscard]] reference operator*() const noexcept {
            assert(node_ != nullptr);
            return node_->value;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            assert(node_ != nullptr);
            return &(node_->value);
        }

    private:
        Node* node_ = nullptr;
    };

public:
    using Iterator = BasicIterator<Type>;
    using ConstIterator = BasicIterator<const Type>;

public:
    // Конструктор по умолчанию
    SingleLinkedList() : head_(), size_(0) {}

    // Конструктор из initializer_list
    SingleLinkedList(std::initializer_list<Type> values) : head_(), size_(0) {
        InitFromRange(values.begin(), values.end());
    }

    // Конструктор копирования
    SingleLinkedList(const SingleLinkedList& other) : head_(), size_(0) {
        InitFromRange(other.begin(), other.end());
    }
    
    // Оператор присваивания
    SingleLinkedList& operator=(const SingleLinkedList& rhs) {
        if (this != &rhs) {
            SingleLinkedList temp(rhs);
            swap(temp);
        }
        return *this;
    }

    // Деструктор
    ~SingleLinkedList() {
        Clear();
    }

    // Вставляет элемент в начало списка
    void PushFront(const Type& value) {
        Node* new_node = new Node(value, head_.next_node);
        head_.next_node = new_node;
        ++size_;
    }

    // Очищает список
    void Clear() noexcept {
        while (head_.next_node != nullptr) {
            Node* node_to_delete = head_.next_node;
            head_.next_node = node_to_delete->next_node;
            delete node_to_delete;
        }
        size_ = 0;
    }

    // Возвращает количество элементов
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает true, если список пуст
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Удаляет первый элемент списка
    void PopFront() noexcept {
        assert(head_.next_node != nullptr);
        Node* node_to_delete = head_.next_node;
        head_.next_node = node_to_delete->next_node;
        delete node_to_delete;
        --size_;
    }

    // Вставка элемента после указанного итератора
    Iterator InsertAfter(ConstIterator pos, const Type& value) {
        assert(pos.node_ != nullptr);
        Node* new_node = new Node(value, pos.node_->next_node);
        pos.node_->next_node = new_node;
        ++size_;
        return Iterator{new_node};
    }

    // Удаляет элемент после указанного итератора
    Iterator EraseAfter(ConstIterator pos) noexcept {
        assert(pos.node_ != nullptr && pos.node_->next_node != nullptr && size_ > 0);
        
        Node* node_to_delete = pos.node_->next_node;

        pos.node_->next_node = node_to_delete->next_node;
        delete node_to_delete;
        --size_;

        return Iterator{pos.node_->next_node};
    }

    [[nodiscard]] Iterator before_begin() noexcept {
        return Iterator{&head_};
    }

    [[nodiscard]] ConstIterator before_begin() const noexcept {
        return ConstIterator{&head_};
    }

    [[nodiscard]] ConstIterator cbefore_begin() const noexcept {
        return ConstIterator{&head_};
    }

    void swap(SingleLinkedList& other) noexcept {
        std::swap(head_.next_node, other.head_.next_node);
        std::swap(size_, other.size_);
    }

    [[nodiscard]] Iterator begin() noexcept {
        return Iterator{head_.next_node};
    }

    [[nodiscard]] Iterator end() noexcept {
        return Iterator{static_cast<Node*>(nullptr)};
    }

    [[nodiscard]] ConstIterator begin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator end() const noexcept {
        return ConstIterator{static_cast<Node*>(nullptr)};
    }

    [[nodiscard]] ConstIterator cbegin() const noexcept {
        return ConstIterator{head_.next_node};
    }

    [[nodiscard]] ConstIterator cend() const noexcept {
        return ConstIterator{static_cast<Node*>(nullptr)};
    }

private:
    Node head_;
    size_t size_ = 0;

    // Шаблонный метод для инициализации из любого диапазона
    template <typename InputIt>
    void InitFromRange(InputIt first, InputIt last) {
        SingleLinkedList tmp;  // Временный список

        Node* tail = &tmp.head_;  // Начинаем с головного узла временного списка
        for (auto it = first; it != last; ++it) {
            tail->next_node = new Node(*it, nullptr);  // Это может вызвать исключение
            tail = tail->next_node;
        }
        tmp.size_ = std::distance(first, last);

        // Если все прошло успешно, меняем местами tmp с текущим списком.
        swap(tmp);  // Это поменяет местами содержимое tmp с *this
        // tmp будет автоматически уничтожен, очистив старый список
    }
};

// Внешние функции

// Функция swap
template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
    lhs.swap(rhs);
}

// Операторы сравнения
template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    if (&lhs == &rhs) {
        return true; // Это один и тот же контейнер
    }

    if (lhs.GetSize() != rhs.GetSize()) {
        return false; // Несоответствие размера
    }
    
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(rhs < lhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
    return !(lhs < rhs);
}