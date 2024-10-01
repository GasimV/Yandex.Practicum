#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

// Tentacle class definition
class Tentacle {
public:
    explicit Tentacle(int id) noexcept
        : id_(id) {
    }

    int GetId() const noexcept {
        return id_;
    }

    Tentacle* GetLinkedTentacle() const noexcept {
        return linked_tentacle_;
    }

    void LinkTo(Tentacle& tentacle) noexcept {
        linked_tentacle_ = &tentacle;
    }

    void Unlink() noexcept {
        linked_tentacle_ = nullptr;
    }

private:
    int id_ = 0;
    Tentacle* linked_tentacle_ = nullptr;
};

// PtrVector class implementation
template <typename T>
class PtrVector {
public:
    PtrVector() = default;

    // Copy constructor
    PtrVector(const PtrVector& other) {
        items_.reserve(other.items_.size());
        try {
            for (auto p : other.items_) {
                auto p_copy = p ? new T(*p) : nullptr;
                items_.push_back(p_copy);
            }
        } catch (...) {
            DeleteItems();
            throw;
        }
    }

    // Destructor
    ~PtrVector() {
        DeleteItems();
    }

    // Copy assignment operator using copy-and-swap idiom
    PtrVector& operator=(const PtrVector& rhs) {
        if (this != &rhs) {
            PtrVector temp(rhs); // Create a copy of rhs
            swap(temp);          // Swap this with temp
        }
        return *this;
    }

    // Method to swap the contents of two PtrVector objects
    void swap(PtrVector& other) noexcept {
        items_.swap(other.items_); // Swap the vectors; this operation is noexcept
    }

    // Returns a reference to the vector of pointers
    vector<T*>& GetItems() noexcept {
        return items_;
    }

    // Returns a const reference to the vector of pointers
    vector<T*> const& GetItems() const noexcept {
        return items_;
    }

private:
    // Method to delete all the elements pointed to by the vector
    void DeleteItems() noexcept {
        for (auto p : items_) {
            delete p;
        }
    }

    vector<T*> items_;
};

// Swap function overload for PtrVector
template <typename T>
void swap(PtrVector<T>& lhs, PtrVector<T>& rhs) noexcept {
    lhs.swap(rhs);
}

// Octopus
class Octopus {
public:
    Octopus() : Octopus(8) {}

    // Constructor with specified number of tentacles
    explicit Octopus(int num_tentacles) {
        tentacles_.GetItems().reserve(num_tentacles);
        try {
            for (int i = 1; i <= num_tentacles; ++i) {
                tentacles_.GetItems().push_back(new Tentacle(i)); // Adding tentacles
            }
        } catch (...) {
            throw; // Rethrow any exception
        }
    }

    // Copy constructor that performs deep copy
    Octopus(const Octopus& other) : tentacles_(other.tentacles_) {}

    // Copy assignment operator using copy-and-swap idiom
    Octopus& operator=(const Octopus& rhs) {
        if (this != &rhs) {
            Octopus temp(rhs); // Create a copy of rhs
            swap(temp); // Swap the current object with the temporary copy
        }
        return *this;
    }

    // Swap method to exchange contents with another Octopus
    void swap(Octopus& other) noexcept {
        tentacles_.swap(other.tentacles_); // Use the swap method from PtrVector
    }

    // Method to add a new tentacle
    Tentacle& AddTentacle() {
        int new_id = GetTentacleCount() + 1;
        tentacles_.GetItems().push_back(new Tentacle(new_id));
        return *tentacles_.GetItems().back();
    }

    int GetTentacleCount() const noexcept {
        return static_cast<int>(tentacles_.GetItems().size());
    }

    const Tentacle& GetTentacle(size_t index) const {
        return *tentacles_.GetItems().at(index);
    }

    Tentacle& GetTentacle(size_t index) {
        return *tentacles_.GetItems().at(index);
    }

private:
    PtrVector<Tentacle> tentacles_;
};

// Swap function for Octopus
void swap(Octopus& lhs, Octopus& rhs) noexcept {
    lhs.swap(rhs);
}

int main() {
    // Проверка присваивания осьминогов
    {
        Octopus octopus1(3);

        // Настраиваем состояние исходного осьминога
        octopus1.GetTentacle(2).LinkTo(octopus1.GetTentacle(1));

        // До присваивания octopus2 имеет своё собственное состояние
        Octopus octopus2(10);

        octopus2 = octopus1;

        // После присваивания осьминогов щупальца копии имеют то же состояние,
        // что и щупальца присваиваемого объекта
        assert(octopus2.GetTentacleCount() == octopus1.GetTentacleCount());
        for (int i = 0; i < octopus2.GetTentacleCount(); ++i) {
            auto& tentacle1 = octopus1.GetTentacle(i);
            auto& tentacle2 = octopus2.GetTentacle(i);
            assert(&tentacle2 != &tentacle1);
            assert(tentacle2.GetId() == tentacle1.GetId());
            assert(tentacle2.GetLinkedTentacle() == tentacle1.GetLinkedTentacle());
        }
    }

    // Проверка самоприсваивания осьминогов
    {
        Octopus octopus(3);

        // Настраиваем состояние осьминога
        octopus.GetTentacle(0).LinkTo(octopus.GetTentacle(1));

        vector<pair<Tentacle*, Tentacle*>> tentacles;
        // Сохраняем информацию о щупальцах осьминога и его копии
        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            tentacles.push_back({&octopus.GetTentacle(i), octopus.GetTentacle(i).GetLinkedTentacle()});
        }

        // Выполняем самоприсваивание
        octopus = octopus;

        // После самоприсваивания состояние осьминога не должно измениться
        assert(octopus.GetTentacleCount() == static_cast<int>(tentacles.size()));
        for (int i = 0; i < octopus.GetTentacleCount(); ++i) {
            auto& tentacle_with_link = tentacles.at(i);
            assert(&octopus.GetTentacle(i) == tentacle_with_link.first);
            assert(octopus.GetTentacle(i).GetLinkedTentacle() == tentacle_with_link.second);
        }
    }
}