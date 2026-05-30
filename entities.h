#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>

// Принцип SRP: класс отвечает только за хранение данных комнаты
class Room {
public:
    enum class Category { STANDARD, STUDIO, APARTMENT };

    Room(int id, Category cat, double pricePerNight, int maxGuests)
        : id(id), category(cat), pricePerNight(pricePerNight), maxGuests(maxGuests), isAvailable(true) {}

    int getId() const { return id; }
    Category getCategory() const { return category; }
    double getPricePerNight() const { return pricePerNight; }
    int getMaxGuests() const { return maxGuests; }
    bool getAvailable() const { return isAvailable; }

    void setAvailable(bool available) { isAvailable = available; }

    std::string categoryToString() const {
        switch(category) {
            case Category::STANDARD: return "Standard";
            case Category::STUDIO:   return "Studio";
            case Category::APARTMENT:return "Apartment";
        }
        return "Unknown";
    }

private:
    int id;
    Category category;
    double pricePerNight;
    int maxGuests;
    bool isAvailable;
};

// SRP: только данные гостя
class Guest {
public:
    Guest(int id, std::string name, std::string phone)
        : id(id), name(name), phone(phone) {}

    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getPhone() const { return phone; }

private:
    int id;
    std::string name;
    std::string phone;
};

// SRP: только данные бронирования
class Booking {
public:
    using time_point = std::chrono::system_clock::time_point;

    Booking(int id, int roomId, int guestId, time_point checkIn, time_point checkOut)
        : id(id), roomId(roomId), guestId(guestId), checkIn(checkIn), checkOut(checkOut) {}

    int getId() const { return id; }
    int getRoomId() const { return roomId; }
    int getGuestId() const { return guestId; }
    time_point getCheckIn() const { return checkIn; }
    time_point getCheckOut() const { return checkOut; }

    long long nights() const {
        auto diff = std::chrono::duration_cast<std::chrono::hours>(checkOut - checkIn).count();
        return (diff + 23) / 24; // округление вверх
    }

private:
    int id;
    int roomId;
    int guestId;
    time_point checkIn;
    time_point checkOut;
};

#endif