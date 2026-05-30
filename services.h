#ifndef SERVICES_H
#define SERVICES_H

#include "repositories.h"
#include "exceptions.h"
#include <memory>
#include <vector>
#include <iostream>

// SRP: сервис управляет только бронированием
class BookingService {
public:
    BookingService(std::shared_ptr<RoomRepository> roomRepo,
                   std::shared_ptr<GuestRepository> guestRepo,
                   std::shared_ptr<BookingRepository> bookingRepo)
        : roomRepo(roomRepo), guestRepo(guestRepo), bookingRepo(bookingRepo) {}

    double calculateCost(int bookingId) {
        auto* booking = bookingRepo->findById(bookingId);
        if (!booking) throw NotFoundException("Booking not found");
        auto* room = roomRepo->findById(booking->getRoomId());
        if (!room) throw RoomException("Room not found");
        return booking->nights() * room->getPricePerNight();
    }

    void createBooking(int roomId, int guestId, 
                       std::chrono::system_clock::time_point checkIn,
                       std::chrono::system_clock::time_point checkOut) {
        if (checkIn >= checkOut) throw ValidationException("Invalid dates");
        auto* room = roomRepo->findById(roomId);
        if (!room || !room->getAvailable()) throw RoomException("Room not available");
        auto* guest = guestRepo->findById(guestId);
        if (!guest) throw NotFoundException("Guest not found");

        int newId = bookingRepo->getNextId();
        Booking booking(newId, roomId, guestId, checkIn, checkOut);
        bookingRepo->add(booking);
        room->setAvailable(false);
    }

private:
    std::shared_ptr<RoomRepository> roomRepo;
    std::shared_ptr<GuestRepository> guestRepo;
    std::shared_ptr<BookingRepository> bookingRepo;
};

// Отдельный сервис для отчётов (SRP)
class ReportService {
public:
    ReportService(std::shared_ptr<BookingRepository> bookingRepo,
                  std::shared_ptr<RoomRepository> roomRepo,
                  std::shared_ptr<GuestRepository> guestRepo)
        : bookingRepo(bookingRepo), roomRepo(roomRepo), guestRepo(guestRepo) {}

    void printAllBookings() {
        auto bookings = bookingRepo->getAll();
        for (const auto& b : bookings) {
            auto* room = roomRepo->findById(b.getRoomId());
            auto* guest = guestRepo->findById(b.getGuestId());
            std::cout << "Booking #" << b.getId() 
                      << " | Room: " << (room ? room->categoryToString() : "?")
                      << " | Guest: " << (guest ? guest->getName() : "?")
                      << " | Nights: " << b.nights() << "\n";
        }
    }
private:
    std::shared_ptr<RoomRepository> roomRepo;
    std::shared_ptr<GuestRepository> guestRepo;
    std::shared_ptr<BookingRepository> bookingRepo;
};

#endif