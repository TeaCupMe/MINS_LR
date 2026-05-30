#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "entities.h"
#include "exceptions.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;
    virtual void add(const T& item) = 0;
    virtual void remove(int id) = 0;
    virtual T* findById(int id) = 0;
    virtual std::vector<T> getAll() const = 0;
};

class RoomRepository : public IRepository<Room> {
public:
    void add(const Room& room) override {
        if (rooms.find(room.getId()) != rooms.end())
            throw ValidationException("Room already exists");
        rooms.emplace(room.getId(), room);
    }
    void remove(int id) override {
        if (rooms.erase(id) == 0)
            throw NotFoundException("Room not found");
    }
    Room* findById(int id) override {
        auto it = rooms.find(id);
        return (it == rooms.end()) ? nullptr : &it->second;
    }
    std::vector<Room> getAll() const override {
        std::vector<Room> result;
        for (const auto& p : rooms) result.push_back(p.second);
        return result;
    }
    int getNextId() const {
        if (rooms.empty()) return 1;
        int maxId = 0;
        for (const auto& p : rooms) if (p.first > maxId) maxId = p.first;
        return maxId + 1;
    }
private:
    std::unordered_map<int, Room> rooms;
};

class GuestRepository : public IRepository<Guest> {
public:
    void add(const Guest& guest) override {
        if (guests.find(guest.getId()) != guests.end())
            throw ValidationException("Guest already exists");
        guests.emplace(guest.getId(), guest);
    }
    void remove(int id) override {
        if (guests.erase(id) == 0)
            throw NotFoundException("Guest not found");
    }
    Guest* findById(int id) override {
        auto it = guests.find(id);
        return (it == guests.end()) ? nullptr : &it->second;
    }
    std::vector<Guest> getAll() const override {
        std::vector<Guest> result;
        for (const auto& p : guests) result.push_back(p.second);
        return result;
    }
    int getNextId() const {
        if (guests.empty()) return 1;
        int maxId = 0;
        for (const auto& p : guests) if (p.first > maxId) maxId = p.first;
        return maxId + 1;
    }
private:
    std::unordered_map<int, Guest> guests;
};

class BookingRepository : public IRepository<Booking> {
public:
    void add(const Booking& booking) override {
        if (bookings.find(booking.getId()) != bookings.end())
            throw ValidationException("Booking already exists");
        bookings.emplace(booking.getId(), booking);
    }
    void remove(int id) override {
        if (bookings.erase(id) == 0)
            throw NotFoundException("Booking not found");
    }
    Booking* findById(int id) override {
        auto it = bookings.find(id);
        return (it == bookings.end()) ? nullptr : &it->second;
    }
    std::vector<Booking> getAll() const override {
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