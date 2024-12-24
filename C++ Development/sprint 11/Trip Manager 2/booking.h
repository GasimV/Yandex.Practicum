#pragma once

namespace raii {

// Template class for handling bookings with RAII
template <typename Provider>
class Booking {
private:
    using BookingId = typename Provider::BookingId;

    Provider* provider_; // Pointer to the booking provider
    BookingId id_; // ID of the booking

public:
    // Constructor to initialize booking with a provider and an ID
    Booking(Provider* provider, BookingId id) : provider_(provider), id_(id) {}

    // Copy constructor is deleted to prevent duplicate ownership
    Booking(const Booking&) = delete;

    // Move constructor transfers ownership of the booking
    Booking(Booking&& other) noexcept : provider_(other.provider_), id_(other.id_) {
        // Nullify the source pointer to avoid double release
        other.provider_ = nullptr;
    }

    // Copy assignment is deleted to prevent duplicate ownership
    Booking& operator=(const Booking&) = delete;

    // Move assignment transfers ownership of the booking
    Booking& operator=(Booking&& other) noexcept {
        if (this != &other) {
            if (provider_) {
                // Cancel the current booking if any
                provider_->CancelOrComplete(*this);
            }
            provider_ = other.provider_;
            id_ = other.id_;
            other.provider_ = nullptr; // Nullify the source pointer
        }
        return *this;
    }

    // Destructor cancels the booking if it has not been completed
    ~Booking() {
        if (provider_) {
            provider_->CancelOrComplete(*this);
        }
    }

    // Эта функция не требуется в тестах, но в реальной программе она может быть нужна (Returns the booking ID)
    BookingId GetId() const {
        return id_;
    } 
};

} // namespace raii