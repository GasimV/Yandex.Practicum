#include <iostream>
#include <vector>

using namespace std;

// Tentacle
class Tentacle {
public:
    explicit Tentacle(int id)
        : id_(id) {
    }

    int GetId() const {
        return id_;
    }

private:
    int id_ = 0;
};

// Octopus
class Octopus {
public:
    Octopus() {
        Tentacle* t = nullptr;
        try {
            for (int i = 1; i <= 8; ++i) {
                t = new Tentacle(i);      // Может выбросить исключение bad_alloc
                tentacles_.push_back(t);  // Может выбросить исключение bad_alloc
                t = nullptr;
            }
        } catch (const bad_alloc&) {
            Cleanup();
            delete t;
            throw;
        }
    }

    // Custom Copy Constructor for deep copying
    Octopus(const Octopus& other) {
        for (Tentacle* t : other.tentacles_) {
            tentacles_.push_back(new Tentacle(t->GetId())); // Create a deep copy of each tentacle
        }
    }

    // Copy assignment operator for deep copying
    Octopus& operator=(const Octopus& other) {
        if (this != &other) {
            Cleanup(); // Clean up existing resources
            for (Tentacle* t : other.tentacles_) {
                tentacles_.push_back(new Tentacle(t->GetId())); // Create a deep copy
            }
        }
        return *this;
    }

    const Tentacle& GetTentacle(int index) const {
        if (index < 0 || static_cast<size_t>(index) >= tentacles_.size()) {
            throw out_of_range("Invalid tentacle index"s);
        }
        return *tentacles_[index];
    }

    ~Octopus() {
        Cleanup();
    }

private:
    void Cleanup() {
        for (Tentacle* t : tentacles_) {
            delete t;
        }
        tentacles_.clear();
    }

    vector<Tentacle*> tentacles_;
};

int main() {
    Octopus originalOctopus;

    // Experiment by making multiple copies in a loop
    for (int i = 1; i <= 5; ++i) {
        Octopus copiedOctopus = originalOctopus; // Now performs a deep copy

        // Accessing the copied tentacle to observe behavior
        try {
            cout << "Copied Octopus " << i << " Tentacle ID: " 
                 << copiedOctopus.GetTentacle(3).GetId() << endl;
        } catch (const std::exception& e) {
            cout << "Error accessing tentacle: " << e.what() << endl;
        }
    }

    cout << "Experiment completed!" << endl;
}