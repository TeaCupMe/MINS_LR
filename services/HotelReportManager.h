#ifndef HOTEL_REPORT_MANAGER_H
#define HOTEL_REPORT_MANAGER_H

#include "../repositories.h"
#include "BookingService.h"
#include <iostream>
#include <memory>
#include <string>

// God Object: отчёты, аналитика, расчёт выручки и уведомления в одном классе (нарушение SRP)
class HotelReportManager {
public:
    HotelReportManager(std::shared_ptr<BookingRepository> bookingRepo,
                       std::shared_ptr<RoomRepository> roomRepo,
                       std::shared_ptr<GuestRepository> guestRepo,
                       std::shared_ptr<BookingService> bookingService)
        : bookingRepo(bookingRepo), roomRepo(roomRepo),
          guestRepo(guestRepo), bookingService(bookingService) {}

    void printFullReport(int revenueMode) {
        std::cout << "\n--- HOTEL FULL REPORT ---\n";
        printBookingsList();
        printOccupancy();
        double revenue = calculateTotalRevenue(revenueMode);
        std::cout << "Total revenue: $" << revenue << "\n";
        std::cout << "--- END REPORT ---\n";
    }

    void notifyAdmin(int channel, const std::string& message) {
        if (channel == 1)
            std::cout << "[EMAIL] " << message << "\n";
        else if (channel == 2)
            std::cout << "[SMS] " << message << "\n";
        else
            std::cout << "[CONSOLE] " << message << "\n";
    }

private:
    void printBookingsList() {
        std::cout << "\nBookings:\n";
        for (const auto& b : bookingRepo->getAll()) {
            auto* room = roomRepo->findById(b.getRoomId());
            auto* guest = guestRepo->findById(b.getGuestId());
            auto extBooking = bookingService->getExtendedBooking(b.getId());
            std::cout << "  #" << b.getId()
                      << " | Room: " << (room ? room->categoryToString() : "?")
                      << " | Guest: " << (guest ? guest->getName() : "?")
                      << " | Nights: " << b.nights()
                      << " | State: " << (extBooking ? extBooking->getState() : "?")
                      << "\n";
        }
    }

    void printOccupancy() {
        auto rooms = roomRepo->getAll();
        if (rooms.empty()) {
            std::cout << "\nOccupancy: no rooms\n";
            return;
        }
        int occupied = 0;
        for (const auto& r : rooms) {
            if (!r.getAvailable()) ++occupied;
        }
        double percent = 100.0 * occupied / rooms.size();
        std::cout << "\nOccupancy: " << occupied << "/" << rooms.size()
                  << " (" << percent << "%)\n";
    }

    // OCP violation: switch вместо Strategy — новый режим требует правки этого метода
    double calculateTotalRevenue(int mode) {
        double total = 0;
        for (const auto& b : bookingRepo->getAll()) {
            auto* room = roomRepo->findById(b.getRoomId());
            if (!room) continue;
            long nights = b.nights();
            switch (mode) {
                case 1: total += room->getPricePerNight() * nights; break;
                case 2: total += room->getPricePerNight() * nights * 0.9; break;
                case 3: total += room->getPricePerNight() * nights * 1.3; break;
            }
        }
        return total;
    }

    std::shared_ptr<BookingRepository> bookingRepo;
    std::shared_ptr<RoomRepository> roomRepo;
    std::shared_ptr<GuestRepository> guestRepo;
    std::shared_ptr<BookingService> bookingService;
};

#endif
