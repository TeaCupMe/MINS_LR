#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "../repositories.h"
#include "../ExtendedBooking.h"
#include "../exceptions.h"
#include <memory>
#include <vector>
#include <unordered_map>

class BookingService {
public:
    BookingService(std::shared_ptr<RoomRepository> roomRepo,
                   std::shared_ptr<GuestRepository> guestRepo,
                   std::shared_ptr<BookingRepository> bookingRepo)
        : roomRepo(roomRepo), guestRepo(guestRepo), bookingRepo(bookingRepo) {}

    int createBooking(int roomId, int guestId,
                      std::chrono::system_clock::time_point checkIn,
                      std::chrono::system_clock::time_point checkOut) {
        auto* room = roomRepo->findById(roomId);
        if (!room || !room->getAvailable())
            throw RoomException("Room not available");
        auto* guest = guestRepo->findById(guestId);
        if (!guest)
            throw NotFoundException("Guest not found");
        if (checkIn >= checkOut)
            throw ValidationException("Invalid dates");

        int newId = bookingRepo->getNextId();
        ExtendedBooking extBooking(newId, roomId, guestId, checkIn, checkOut, room->getPricePerNight());
        Booking baseBooking(newId, roomId, guestId, checkIn, checkOut);
        bookingRepo->add(baseBooking);
        extBookings[newId] = std::make_shared<ExtendedBooking>(extBooking);
        room->setAvailable(false);
        return newId;
    }

    std::shared_ptr<ExtendedBooking> getExtendedBooking(int id) {
        auto it = extBookings.find(id);
        if (it == extBookings.end()) return nullptr;
        return it->second;
    }
    std::vector<std::shared_ptr<ExtendedBooking>> getAllBookings() const 
    {
        std::vector<std::shared_ptr<ExtendedBooking>> result;
        for (const auto& pair : extBookings) {
            result.push_back(pair.second);
        }
        return result;
    }

private:
    std::shared_ptr<RoomRepository> roomRepo;
    std::shared_ptr<GuestRepository> guestRepo;
    std::shared_ptr<BookingRepository> bookingRepo;
    std::unordered_map<int, std::shared_ptr<ExtendedBooking>> extBookings;
};

#endif