#include <iostream>
#include <map>
#include <deque>
#include <set>
#include <string>
#include <cstdint>

using namespace std;

struct Booking {
    int64_t time;
    int client_id;
    int room_count;
};

class HotelManager {
public:
    void Book(int64_t time, const string& hotel_name, int client_id, int room_count) {
        auto& bookings = hotel_bookings[hotel_name];
        bookings.push_back({time, client_id, room_count});
        current_time = time;

        // Clean up old bookings
        while (!bookings.empty() && bookings.front().time <= time - SECONDS_IN_DAY) {
            const auto& old_booking = bookings.front();
            room_count_by_hotel[hotel_name] -= old_booking.room_count;
            client_bookings[hotel_name][old_booking.client_id]--;
            if (client_bookings[hotel_name][old_booking.client_id] == 0) {
                client_bookings[hotel_name].erase(old_booking.client_id);
            }
            bookings.pop_front();
        }

        // Add new booking
        room_count_by_hotel[hotel_name] += room_count;
        client_bookings[hotel_name][client_id]++;
    }

    int ComputeClientCount(const string& hotel_name) {
        RemoveOutdatedBookings(hotel_name);
        return client_bookings[hotel_name].size();
    }

    int ComputeRoomCount(const string& hotel_name) {
        RemoveOutdatedBookings(hotel_name);
        return room_count_by_hotel[hotel_name];
    }

private:
    static const int64_t SECONDS_IN_DAY = 86400;
    int64_t current_time = 0;

    // Hotel data
    map<string, deque<Booking>> hotel_bookings;
    map<string, map<int, int>> client_bookings;
    map<string, int> room_count_by_hotel;

    void RemoveOutdatedBookings(const string& hotel_name) {
        auto& bookings = hotel_bookings[hotel_name];
        while (!bookings.empty() && bookings.front().time <= current_time - SECONDS_IN_DAY) {
            const auto& old_booking = bookings.front();
            room_count_by_hotel[hotel_name] -= old_booking.room_count;
            client_bookings[hotel_name][old_booking.client_id]--;
            if (client_bookings[hotel_name][old_booking.client_id] == 0) {
                client_bookings[hotel_name].erase(old_booking.client_id);
            }
            bookings.pop_front();
        }
    }
};

int main() {
    HotelManager manager;

    int query_count;
    cin >> query_count;

    for (int query_id = 0; query_id < query_count; ++query_id) {
        string query_type;
        cin >> query_type;

        if (query_type == "BOOK") {
            int64_t time;
            string hotel_name;
            int client_id, room_count;
            cin >> time >> hotel_name >> client_id >> room_count;
            manager.Book(time, hotel_name, client_id, room_count);
        } else {
            string hotel_name;
            cin >> hotel_name;
            if (query_type == "CLIENTS") {
                cout << manager.ComputeClientCount(hotel_name) << "\n";
            } else if (query_type == "ROOMS") {
                cout << manager.ComputeRoomCount(hotel_name) << "\n";
            }
        }
    }

    return 0;
}