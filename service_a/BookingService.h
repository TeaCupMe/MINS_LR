#ifndef BOOKING_SERVICE_H
#define BOOKING_SERVICE_H

#include "../ExtendedBooking.h"
#include "../exceptions.h"
#include "ReferenceClient.h"
#include "repositories.h"
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

class BookingService {
public:
    BookingService(std::shared_ptr<ReferenceClient> referenceClient,
                   std::shared_ptr<BookingRepository> bookingRepo)
        : referenceClient(referenceClient), bookingRepo(bookingRepo) {}

    struct CreateResult {
        bool success = false;
        std::string message;
        int bookingId = 0;
    };

    CreateResult createBooking(int roomId, int guestId,
                               std::chrono::system_clock::time_point checkIn,
                               std::chrono::system_clock::time_point checkOut,
                               const std::string& traceId) {
        if (checkIn >= checkOut) {
            return {false, "Invalid dates", 0};
        }

        auto validation = referenceClient->validateBooking(roomId, guestId, traceId);
        if (!validation.serviceAvailable) {
            return {false, "Reference service unavailable. Please try again later.", 0};
        }
        if (!validation.valid) {
            return {false, validation.errorMessage, 0};
        }

        int newId = bookingRepo->getNextId();
        ExtendedBooking extBooking(newId, roomId, guestId, checkIn, checkOut, validation.pricePerNight);
        Booking baseBooking(newId, roomId, guestId, checkIn, checkOut);
        bookingRepo->add(baseBooking);
        extBookings[newId] = std::make_shared<ExtendedBooking>(extBooking);

        if (!referenceClient->setRoomAvailability(roomId, false, traceId)) {
            extBookings.erase(newId);
            bookingRepo->remove(newId);
            return {false, "Reference service unavailable. Please try again later.", 0};
        }

        return {true, "Booking created", newId};
    }

    std::shared_ptr<ExtendedBooking> getExtendedBooking(int id) {
        auto it = extBookings.find(id);
        if (it == extBookings.end()) return nullptr;
        return it->second;
    }

    std::vector<std::shared_ptr<ExtendedBooking>> getAllBookings() const {
        std::vector<std::shared_ptr<ExtendedBooking>> result;
        for (const auto& pair : extBookings) {
            result.push_back(pair.second);
        }
        return result;
    }

    std::shared_ptr<BookingRepository> getBookingRepo() const { return bookingRepo; }

    bool releaseRoom(int roomId, const std::string& traceId) {
        return referenceClient->setRoomAvailability(roomId, true, traceId);
    }

private:
    std::shared_ptr<ReferenceClient> referenceClient;
    std::shared_ptr<BookingRepository> bookingRepo;
    std::unordered_map<int, std::shared_ptr<ExtendedBooking>> extBookings;
};

#endif
