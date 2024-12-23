#include <cassert>
#include <cstddef> // for nullptr_t
#include <utility> // for std::exchange

using namespace std;

// Template Class UniquePtr
template <typename T>
class UniquePtr {

private:
    T* ptr_ = nullptr; // Pointer to the managed object

public:
    // Default constructor: creates an empty UniquePtr
    UniquePtr() : ptr_(nullptr) {}

    // Constructor that takes ownership of a raw pointer
    explicit UniquePtr(T* ptr) : ptr_(ptr) {}

    // Deleted copy constructor to prevent copying
    UniquePtr(const UniquePtr&) = delete;

    // Move constructor: transfers ownership
    UniquePtr(UniquePtr&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    // Deleted copy assignment operator to prevent copying
    UniquePtr& operator=(const UniquePtr&) = delete;

    // Move assignment operator: transfers ownership
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {
            Reset(nullptr); // Release current resource
            ptr_ = other.ptr_;
            other.ptr_ = nullptr;
        }
        return *this;
    }

    // Assignment operator for nullptr
    UniquePtr& operator=(nullptr_t) {
        Reset(nullptr);
        return *this;
    }

    // Destructor: releases the managed object
    ~UniquePtr() {
        delete ptr_;
    }

    // Dereference operator: access the managed object
    T& operator*() const {
        assert(ptr_ != nullptr && "Dereferencing null pointer");
        return *ptr_;
    }

    // Arrow operator: access members of the managed object
    T* operator->() const {
        assert(ptr_ != nullptr && "Accessing members of null pointer");
        return ptr_;
    }

    // Release ownership and return the raw pointer
    T* Release() {
        T* oldPtr = ptr_;
        ptr_ = nullptr;
        return oldPtr;
    }

    // Reset the managed object with a new pointer
    void Reset(T* ptr) {
        if (ptr_ != ptr) {
            delete ptr_;
            ptr_ = ptr;
        }
    }

    // Swap the managed object with another UniquePtr
    void Swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    // Get the raw pointer
    T* Get() const {
        return ptr_;
    }
};

struct Item {
    static int counter;
    int value;
    Item(int v = 0)
        : value(v) 
    {
        ++counter;
    }
    Item(const Item& other)
        : value(other.value) 
    {
        ++counter;
    }
    ~Item() {
        --counter;
    }
};

int Item::counter = 0;

void TestLifetime() {
    Item::counter = 0;
    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        ptr.Reset(new Item);
        assert(Item::counter == 1);
    }
    assert(Item::counter == 0);

    {
        UniquePtr<Item> ptr(new Item);
        assert(Item::counter == 1);

        auto rawPtr = ptr.Release();
        assert(Item::counter == 1);

        delete rawPtr;
        assert(Item::counter == 0);
    }
    assert(Item::counter == 0);
}

void TestGetters() {
    UniquePtr<Item> ptr(new Item(42));
    assert(ptr.Get()->value == 42);
    assert((*ptr).value == 42);
    assert(ptr->value == 42);
}

int main() {
    TestLifetime();
    TestGetters();
}