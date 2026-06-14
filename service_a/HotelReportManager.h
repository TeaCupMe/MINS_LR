#ifndef HOTEL_REPORT_MANAGER_H
#define HOTEL_REPORT_MANAGER_H

#include "BookingService.h"
#include "ReferenceClient.h"
#include <memory>
#include <sstream>
#include <string>

class HotelReportManager {
public:
    HotelReportManager(std::shared_ptr<BookingRepository> bookingRepo,
                       std::shared_ptr<ReferenceClient> referenceClient,
                       std::shared_ptr<BookingService> bookingService)
        : bookingRepo(bookingRepo), referenceClient(referenceClient),
          bookingService(bookingService) {}

    struct ReportResult {
        bool success = false;
        std::string message;
        std::string reportText;
    };

    ReportResult buildFullReport(int revenueMode, const std::string& traceId) {
        ReportResult result;
        std::ostringstream report;
        report << "\n--- HOTEL FULL REPORT ---\n";

        auto rooms = referenceClient->listRooms(traceId);
        auto guests = referenceClient->listGuests(traceId);
        if (!rooms || !guests) {
            result.success = false;
            result.message = "Reference service unavailable. Report data incomplete.";
            report << "\nReference data unavailable.\n";
            appendBookingsList(report, nullptr, nullptr, traceId);
            result.reportText = report.str();
            return result;
        }

        appendBookingsList(report, &(*rooms), &(*guests), traceId);
        appendOccupancy(report, *rooms);
        double revenue = calculateTotalRevenue(revenueMode, *rooms);
        report << "Total revenue: $" << revenue << "\n";
        report << "--- END REPORT ---\n";

        result.success = true;
        result.reportText = report.str();
        return result;
    }

    std::string notifyAdmin(int channel, const std::string& message) {
        if (channel == 1) return "[EMAIL] " + message;
        if (channel == 2) return "[SMS] " + message;
        return "[CONSOLE] " + message;
    }

private:
    void appendBookingsList(std::ostringstream& report,
                            const std::vector<RoomInfo>* rooms,
                            const std::vector<GuestInfo>* guests,
                            const std::string& traceId) {
        report << "\nBookings:\n";
        for (const auto& b : bookingRepo->getAll()) {
            std::string roomName = "?";
            std::string guestName = "?";
            if (rooms) {
                for (const auto& r : *rooms) {
                    if (r.id == b.getRoomId()) { roomName = r.category; break; }
                }
            }
            if (guests) {
                for (const auto& g : *guests) {
                    if (g.id == b.getGuestId()) { guestName = g.name; break; }
                }
            }
            auto extBooking = bookingService->getExtendedBooking(b.getId());
            report << "  #" << b.getId()
                   << " | Room: " << roomName
                   << " | Guest: " << guestName
                   << " | Nights: " << b.nights()
                   << " | State: " << (extBooking ? extBooking->getState() : "?")
                   << "\n";
        }
    }

    void appendOccupancy(std::ostringstream& report, const std::vector<RoomInfo>& rooms) {
        if (rooms.empty()) {
            report << "\nOccupancy: no rooms\n";
            return;
        }
        int occupied = 0;
        for (const auto& r : rooms) {
            if (!r.available) ++occupied;
        }
        double percent = 100.0 * occupied / rooms.size();
        report << "\nOccupancy: " << occupied << "/" << rooms.size()
               << " (" << percent << "%)\n";
    }

    double calculateTotalRevenue(int mode, const std::vector<RoomInfo>& rooms) {
        double total = 0;
        for (const auto& b : bookingRepo->getAll()) {
            const RoomInfo* room = nullptr;
            for (const auto& r : rooms) {
                if (r.id == b.getRoomId()) { room = &r; break; }
            }
            if (!room) continue;
            long nights = b.nights();
            switch (mode) {
                case 1: total += room->pricePerNight * nights; break;
                case 2: total += room->pricePerNight * nights * 0.9; break;
                case 3: total += room->pricePerNight * nights * 1.3; break;
            }
        }
        return total;
    }

    std::shared_ptr<BookingRepository> bookingRepo;
    std::shared_ptr<ReferenceClient> referenceClient;
    std::shared_ptr<BookingService> bookingService;
};

#endif
