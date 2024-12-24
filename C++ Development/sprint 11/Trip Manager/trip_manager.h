#pragma once
#include "flight_provider.h"
#include "hotel_provider.h"

#include <string>
#include <vector>
#include <utility>

class Trip {
public:
    // Constructor initializes references to the providers
    Trip(HotelProvider& hp, FlightProvider& fp) : hotel_provider_(hp), flight_provider_(fp) {}

    // Copy operations are explicitly deleted as this class manages resources
    Trip(const Trip&) = delete;
    Trip& operator=(const Trip&) = delete;

    // Move constructor transfers ownership of bookings and clears the source
    Trip(Trip&& other) noexcept
        : hotel_provider_(other.hotel_provider_),
          flight_provider_(other.flight_provider_),
          hotels_(std::move(other.hotels_)),
          flights_(std::move(other.flights_)) {
            // Clear the source to prevent accidental cancellations
            other.hotels_.clear();
            other.flights_.clear();
          }

    // Move assignment operator ensures proper resource cleanup before transfer
    Trip& operator=(Trip&& other) noexcept {
        if (this != &other) {
            Cancel(); // Cancel existing bookings to avoid resource leaks
            hotel_provider_ = other.hotel_provider_;
            flight_provider_ = other.flight_provider_;
            hotels_ = std::move(other.hotels_);
            flights_ = std::move(other.flights_);
            // Clear the source object
            other.hotels_.clear();
            other.flights_.clear();
        }
        return *this;
    }

    // Destructor automatically cancels all bookings
    ~Trip() {
        Cancel();
    }

    // Add a hotel booking ID to the list of active bookings
    void AddHotelBooking(HotelProvider::BookingId id) {
        hotels_.push_back(id);
    }

     // Add a flight booking ID to the list of active bookings
     void AddFlightBooking(FlightProvider::BookingId id) {
        flights_.push_back(id);
     }

     // Cancel all active bookings in reverse order of addition
     void Cancel() {
        for (const auto& id : flights_) {
            flight_provider_.Cancel(id); // Cancel each flight booking
        }
        flights_.clear(); // Clear the list of flight bookings

        for (const auto& id : hotels_) {
            hotel_provider_.Cancel(id); // Cancel each hotel booking
        }
        hotels_.clear(); // Clear the list of hotel bookings
     }

private:
    // References to the providers, shared among all trips
    HotelProvider& hotel_provider_;
    FlightProvider& flight_provider_;

    // Lists of active booking IDs for flights and hotels
    std::vector<HotelProvider::BookingId> hotels_;
    std::vector<FlightProvider::BookingId> flights_;
};

class TripManager {
public:
    using BookingId = std::string;
    struct BookingData {
        std::string city_from;
        std::string city_to;
        std::string date_from;
        std::string date_to;
    };

    // Book a trip using the provided data
    Trip Book(const BookingData& data) {
        (void) data;
        Trip trip(hotel_provider_, flight_provider_); // Create a Trip instance

        // Book a flight from city_from to city_to
        FlightProvider::BookingData flight_booking_data;
        trip.AddFlightBooking(flight_provider_.Book(flight_booking_data));

        // Book a hotel stay at the destination
        HotelProvider::BookingData hotel_booking_data;
        trip.AddHotelBooking(hotel_provider_.Book(hotel_booking_data));

        // Book a return flight from city_to to city_from
        trip.AddFlightBooking(flight_provider_.Book(flight_booking_data));

        return trip; // Return the successfully booked trip
    }

private:
    // Providers for hotel and flight bookings
    HotelProvider hotel_provider_;
    FlightProvider flight_provider_;
};