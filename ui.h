#ifndef UI_H
#define UI_H

#include "services.h"
#include <iostream>
#include <ctime>

class ConsoleUI {
public:
    ConsoleUI(std::shared_ptr<BookingService> bookingSvc,
              std::shared_ptr<ReportService> reportSvc,
              std::shared_ptr<RoomRepository> roomRepo,
              std::shared_ptr<GuestRepository> guestRepo)
        : bookingSvc(bookingSvc), reportSvc(reportSvc), 
          roomRepo(roomRepo), guestRepo(guestRepo) {}

    void run() {
        int choice;
        do {
            std::cout << "\n=== HOTEL SYSTEM ===\n";
            std::cout << "1. Show rooms\n2. Show guests\n3. New booking\n4. Calculate cost\n5. Report\n0. Exit\nChoice: ";
            std::cin >> choice;

            try {
                switch(choice) {
                    case 1: showRooms(); break;
                    case 2: showGuests(); break;
                    case 3: newBooking(); break;
                    case 4: calcCost(); break;
                    case 5: reportSvc->printAllBookings(); break;
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
        // Упрощённо: даты задаём через дни от today
        auto now = std::chrono::system_clock::now();
        auto checkIn = now + std::chrono::hours(24);
        auto checkOut = checkIn + std::chrono::hours(48);
        bookingSvc->createBooking(roomId, guestId, checkIn, checkOut);
        std::cout << "Booking created (2 nights from tomorrow)\n";
    }

    void calcCost() {
        int bookingId;
        std::cout << "Booking ID: "; std::cin >> bookingId;
        double cost = bookingSvc->calculateCost(bookingId);
        std::cout << "Total cost: $" << cost << "\n";
    }

    std::shared_ptr<BookingService> bookingSvc;
    std::shared_ptr<ReportService> reportSvc;
    std::shared_ptr<RoomRepository> roomRepo;
    std::shared_ptr<GuestRepository> guestRepo;
};

#endif