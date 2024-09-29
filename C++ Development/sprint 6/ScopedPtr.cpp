#include <cassert>
#include <iostream>
#include <stdexcept> // Include for std::logic_error

// Smart pointer class template, which manages and deletes the associated object upon its destruction
template <typename T>
class ScopedPtr {
public:
    // Default constructor initializes with nullptr
    ScopedPtr() = default;

    // Constructor that initializes the smart pointer with the given raw pointer
    explicit ScopedPtr(T* raw_ptr) noexcept : ptr_(raw_ptr) {
    }

    // Copy constructor is deleted to prevent copying
    ScopedPtr(const ScopedPtr&) = delete;

    // Destructor deletes the owned object if it exists
    ~ScopedPtr() {
        delete ptr_;
    }

    // Returns the raw pointer managed by ScopedPtr
    T* GetRawPtr() const noexcept {
        return ptr_;
    }

    // Releases ownership of the object, sets internal pointer to nullptr, and returns the original pointer
    T* Release() noexcept {
        T* temp = ptr_;
        ptr_ = nullptr;
        return temp;
    }

    // Overloaded operator for explicit conversion to bool
    explicit operator bool() const noexcept {
        return ptr_ != nullptr;
    }

    // Overloaded dereference operator (*) to access the underlying object
    // Throws std::logic_error if the pointer is nullptr
    T& operator*() const {
        if (!ptr_) {
            throw std::logic_error("Dereferencing a null pointer");
        }
        return *ptr_;
    }

    // Overloaded arrow operator (->) to access the members of the underlying object
    // Throws std::logic_error if the pointer is nullptr
    T* operator->() const {
        if (!ptr_) {
            throw std::logic_error("Accessing a member of a null pointer");
        }
        return ptr_;
    }

private:
    T* ptr_ = nullptr; // Raw pointer managed by ScopedPtr
};

// Test the ScopedPtr implementation using the given main function
int main() {
    // Helper struct that sets a flag upon destruction
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // Test automatic deletion
    {
        bool is_deleted = false;
        {
            DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
            ScopedPtr<DeletionSpy> p(raw_ptr);
            assert(p.GetRawPtr() == raw_ptr);
            assert(!is_deleted);
        }
        assert(is_deleted);
    }

    // Test Release method
    {
        bool is_deleted = false;
        DeletionSpy* raw_ptr = new DeletionSpy(is_deleted);
        {
            ScopedPtr<DeletionSpy> scoped_ptr(raw_ptr);
            assert(scoped_ptr.Release() == raw_ptr);
            assert(scoped_ptr.GetRawPtr() == nullptr);
        }
        assert(!is_deleted);
        delete raw_ptr;
        assert(is_deleted);
    }

    // Test operator bool
    {
        const ScopedPtr<int> empty_ptr;
        assert(!empty_ptr);

        const ScopedPtr<int> ptr_to_existing_object(new int(0));
        assert(ptr_to_existing_object);

        static_assert(noexcept(static_cast<bool>(ptr_to_existing_object)));
    }

    // Test operator *
    {
        std::string* raw_ptr = new std::string("hello");
        ScopedPtr<std::string> smart_ptr(raw_ptr);
        assert(&*smart_ptr == raw_ptr);

        try {
            ScopedPtr<int> empty_ptr;
            *empty_ptr;
            assert(false); // Should not reach here
        } catch (const std::logic_error&) {
        } catch (...) {
            assert(false);
        }
    }

    // Test operator ->
    {
        std::string* raw_ptr = new std::string("hello");
        ScopedPtr<std::string> smart_ptr(raw_ptr);
        assert(smart_ptr->data() == raw_ptr->data());

        try {
            ScopedPtr<std::string> empty_ptr;
            empty_ptr->clear();
            assert(false); // Should not reach here
        } catch (const std::logic_error&) {
        } catch (...) {
            assert(false);
        }
    }

    // Example usage
    {
        struct Object {
            Object() {
                std::cout << "Object is default constructed" << std::endl;
            }
            void DoSomething() {
                std::cout << "Doing something" << std::endl;
            }
            ~Object() {
                std::cout << "Object is destroyed" << std::endl;
            }
        };

        ScopedPtr<Object> empty_smart_ptr;
        assert(!empty_smart_ptr);

        ScopedPtr<Object> smart_ptr(new Object());
        assert(smart_ptr);

        (*smart_ptr).DoSomething();
        smart_ptr->DoSomething();
    }
}