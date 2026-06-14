#ifndef SERVICE_A_REPOSITORIES_H
#define SERVICE_A_REPOSITORIES_H

#include "../entities.h"
#include "../exceptions.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

class BookingRepository {
public:
    void add(const Booking& booking) {
        if (bookings.find(booking.getId()) != bookings.end())
            throw ValidationException("Booking already exists");
        bookings.emplace(booking.getId(), booking);
    }
    void remove(int id) {
        if (bookings.erase(id) == 0)
            throw NotFoundException("Booking not found");
    }
    Booking* findById(int id) {
        auto it = bookings.find(id);
        return (it == bookings.end()) ? nullptr : &it->second;
    }
    std::vector<Booking> getAll() const {
        std::vector<Booking> result;
        for (const auto& p : bookings) result.push_back(p.second);
        return result;
    }
    int getNextId() const {
        if (bookings.empty()) return 1;
        int maxId = 0;
        for (const auto& p : bookings) if (p.first > maxId) maxId = p.first;
        return maxId + 1;
    }

private:
    std::unordered_map<int, Booking> bookings;
};

#endif
