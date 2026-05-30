#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "../services/BookingService.h"
#include "../patterns/strategy/PricingStrategy.h"
#include <iostream>
#include <memory>
#include <chrono>

class ConsoleUI {
public:
    ConsoleUI(std::shared_ptr<BookingService> service,
              std::shared_ptr<RoomRepository> roomRepo,
              std::shared_ptr<GuestRepository> guestRepo)
        : bookingService(service), roomRepo(roomRepo), guestRepo(guestRepo) {}

    void run() {
        int choice;
        // Исправлено: явное объявление как shared_ptr<PricingStrategy>
        std::shared_ptr<PricingStrategy> currentStrategy = std::make_shared<StandardPricing>();

        do {
            std::cout << "\n=== HOTEL SYSTEM WITH PATTERNS (SOLID) ===\n";
            std::cout << "1. Show rooms\n2. Show guests\n3. New booking\n";
            std::cout << "4. Confirm booking (State)\n5. Check in (State)\n6. Check out (State)\n7. Cancel booking (State)\n";
            std::cout << "8. Calculate cost (Strategy)\n9. Change pricing strategy\n0. Exit\nChoice: ";
            std::cin >> choice;

            try {
                switch (choice) {
                    case 1: showRooms(); break;
                    case 2: showGuests(); break;
                    case 3: newBooking(); break;
                    case 4: changeState("confirm"); break;
                    case 5: changeState("checkin"); break;
                    case 6: changeState("checkout"); break;
                    case 7: changeState("cancel"); break;
                    case 8: calculateCost(currentStrategy); break;
                    case 9: currentStrategy = chooseStrategy(); break;
                }
            } catch (const HotelSystemException& e) {
                std::cout << "Error: " << e.what() << "\n";
            }
        } while (choice != 0);
    }

private:
    void showRooms() {
        for (const auto& r : roomRepo->getAll()) {
            std::cout << "Room " << r.getId() << " | " << r.categoryToString()
                      << " | $" << r.getPricePerNight() << " | Available: " << r.getAvailable() << "\n";
        }
    }
    void showGuests() {
        for (const auto& g : guestRepo->getAll()) {
            std::cout << "Guest " << g.getId() << ": " << g.getName() << " (" << g.getPhone() << ")\n";
        }
    }
    void newBooking() {
        int roomId, guestId;
        std::cout << "Room ID: "; std::cin >> roomId;
        std::cout << "Guest ID: "; std::cin >> guestId;
        auto now = std::chrono::system_clock::now();
        auto checkIn = now + std::chrono::hours(24);
        auto checkOut = checkIn + std::chrono::hours(48);
        int id = bookingService->createBooking(roomId, guestId, checkIn, checkOut);
        std::cout << "Booking created with ID " << id << " (state: New)\n";
    }
    void changeState(const std::string& action) {
        int id;
        std::cout << "Booking ID: "; std::cin >> id;
        auto booking = bookingService->getExtendedBooking(id);
        if (!booking) throw NotFoundException("Booking not found");
        if (action == "confirm") booking->confirm();
        else if (action == "checkin") booking->checkIn();
        else if (action == "checkout") booking->checkOut();
        else if (action == "cancel") booking->cancel();
    }
    void calculateCost(std::shared_ptr<PricingStrategy> strategy) {
        int id;
        std::cout << "Booking ID: "; std::cin >> id;
        auto booking = bookingService->getExtendedBooking(id);
        if (!booking) throw NotFoundException("Booking not found");
        double cost = booking->calculateTotalCost(*strategy);
        std::cout << "Total cost: $" << cost << "\n";
    }
    std::shared_ptr<PricingStrategy> chooseStrategy() {
        int strat;
        std::cout << "1. Standard\n2. Loyalty discount (10% off)\n3. Holiday (+30%)\nChoice: ";
        std::cin >> strat;
        if (strat == 2) return std::make_shared<LoyaltyDiscountPricing>();
        if (strat == 3) return std::make_shared<HolidayPricing>();
        return std::make_shared<StandardPricing>();
    }

    std::shared_ptr<BookingService> bookingService;
    std::shared_ptr<RoomRepository> roomRepo;
    std::shared_ptr<GuestRepository> guestRepo;
};

#endif