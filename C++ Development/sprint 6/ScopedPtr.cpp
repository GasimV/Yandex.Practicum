#include <cassert>

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
}