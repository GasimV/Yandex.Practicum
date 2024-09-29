#include <algorithm>
#include <cassert>
#include <vector>

using namespace std;

template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    // Copy constructor
    PtrVector(const PtrVector& other) {
        items_.reserve(other.items_.size()); // Reserve memory to prevent exceptions during copying
        for (T* item : other.items_) {
            if (item) {
                items_.push_back(new T(*item)); // Deep copy each non-null pointer
            } else {
                items_.push_back(nullptr); // Keep nullptrs as is
            }
        }
    }

    // Destructor
    ~PtrVector() {
        for (T* item : items_) {
            delete item; // Delete each object pointed to
        }
    }

    // Returns a non-const reference to the vector of pointers
    vector<T*>& GetItems() noexcept {
        return items_;
    }

    // Returns a const reference to the vector of pointers
    vector<T*> const& GetItems() const noexcept {
        return items_;
    }

private:
    vector<T*> items_;
};

// This main function tests the PtrVector template class
int main() {
    // Helper "spy" struct that helps track its deletion
    struct DeletionSpy {
        explicit DeletionSpy(bool& is_deleted)
            : is_deleted_(is_deleted) {
        }
        ~DeletionSpy() {
            is_deleted_ = true;
        }
        bool& is_deleted_;
    };

    // Test element deletion
    {
        bool spy1_is_deleted = false;
        DeletionSpy* ptr1 = new DeletionSpy(spy1_is_deleted);
        {
            PtrVector<DeletionSpy> ptr_vector;
            ptr_vector.GetItems().push_back(ptr1);
            assert(!spy1_is_deleted);

            // Constant reference to ptr_vector
            const auto& const_ptr_vector_ref(ptr_vector);
            // Both const and non-const versions of GetItems should return a reference to the same vector
            assert(&const_ptr_vector_ref.GetItems() == &ptr_vector.GetItems());
        }
        // The PtrVector destructor should delete all objects the pointers point to
        assert(spy1_is_deleted);
    }

    // Helper "spy" struct that tracks its copying
    struct CopyingSpy {
        explicit CopyingSpy(int& copy_count)
            : copy_count_(copy_count) {
        }
        CopyingSpy(const CopyingSpy& rhs)
            : copy_count_(rhs.copy_count_)  //
        {
            ++copy_count_;
        }
        int& copy_count_;
    };

    // Test copying elements when copying a vector of pointers
    {
        // 10 elements
        vector<int> copy_counters(10);

        PtrVector<CopyingSpy> ptr_vector;
        // Prepare the original array of pointers
        for (auto& counter : copy_counters) {
            ptr_vector.GetItems().push_back(new CopyingSpy(counter));
        }
        // The last element contains a nullptr
        ptr_vector.GetItems().push_back(nullptr);

        auto ptr_vector_copy(ptr_vector);
        // The number of elements in the copy should be equal to the number of elements in the original vector
        assert(ptr_vector_copy.GetItems().size() == ptr_vector.GetItems().size());

        // The copy should store pointers to new objects
        assert(ptr_vector_copy.GetItems() != ptr_vector.GetItems());
        // The last element of the original array and its copy should be a nullptr
        assert(ptr_vector_copy.GetItems().back() == nullptr);
        // Check that elements were copied (the copying spies should increment copy counters)
        assert(all_of(copy_counters.begin(), copy_counters.end(), [](int counter) {
            return counter == 1;
        }));
    }
}