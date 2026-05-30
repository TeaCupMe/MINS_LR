#ifndef SERVICES_H
#define SERVICES_H

#include "repositories.h"
#include "exceptions.h"
#include <memory>
#include <vector>
#include <iostream>

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