#pragma once

#include "scopedptr.h" 
#include "ptrvector.h"

#include <new> // Для исключения bad_alloc

// Щупальце
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

// Осьминог
class Octopus {
public:
    Octopus()
        : Octopus(8) {
    }

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

    // Добавляет новое щупальце с идентификатором,
    // равным (количество_щупалец + 1):
    // 1, 2, 3, ...
    // Возвращает ссылку на добавленное щупальце
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