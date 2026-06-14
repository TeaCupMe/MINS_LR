#ifndef CONSOLE_UI_H
#define CONSOLE_UI_H

#include "core.grpc.pb.h"
#include "../shared/trace.h"
#include <chrono>
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

class ConsoleUI {
public:
    explicit ConsoleUI(std::unique_ptr<hotel::CoreService::Stub> stub)
        : stub_(std::move(stub)) {}

    void run() {
        int choice = -1;
        int currentStrategy = 1;

        do {
            std::cout << "\n=== HOTEL SYSTEM WITH PATTERNS (SOLID) ===\n";
            std::cout << "1. Show rooms\n2. Show guests\n3. New booking\n";
            std::cout << "4. Confirm booking (State)\n5. Check in (State)\n6. Check out (State)\n7. Cancel booking (State)\n";
            std::cout << "8. Calculate cost (Strategy)\n9. Change pricing strategy\n";
            std::cout << "10. Hotel report\n11. Notify admin\n0. Exit\nChoice: ";
            std::cin >> choice;

            std::string traceId = trace_util::generateTraceId();

            switch (choice) {
                case 1: showRooms(traceId); break;
                case 2: showGuests(traceId); break;
                case 3: newBooking(traceId); break;
                case 4: changeState("confirm", traceId); break;
                case 5: changeState("checkin", traceId); break;
                case 6: changeState("checkout", traceId); break;
                case 7: changeState("cancel", traceId); break;
                case 8: calculateCost(currentStrategy, traceId); break;
                case 9: currentStrategy = chooseStrategy(); break;
                case 10: showFullReport(traceId); break;
                case 11: sendAdminNotification(traceId); break;
            }
        } while (choice != 0);
    }

private:
    void showRooms(const std::string& traceId) {
        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::Empty request;
        hotel::RoomList response;
        auto status = stub_->ListRooms(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        if (!response.success()) {
            std::cout << response.message() << "\n";
            return;
        }
        for (const auto& r : response.rooms()) {
            std::cout << "Room " << r.id() << " | " << r.category()
                      << " | $" << r.price_per_night()
                      << " | Available: " << r.available() << "\n";
        }
    }

    void showGuests(const std::string& traceId) {
        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::Empty request;
        hotel::GuestList response;
        auto status = stub_->ListGuests(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        if (!response.success()) {
            std::cout << response.message() << "\n";
            return;
        }
        for (const auto& g : response.guests()) {
            std::cout << "Guest " << g.id() << ": " << g.name() << " (" << g.phone() << ")\n";
        }
    }

    void newBooking(const std::string& traceId) {
        int roomId, guestId;
        std::cout << "Room ID: "; std::cin >> roomId;
        std::cout << "Guest ID: "; std::cin >> guestId;

        auto now = std::chrono::system_clock::now();
        auto checkIn = now + std::chrono::hours(24);
        auto checkOut = checkIn + std::chrono::hours(48);

        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::CreateBookingRequest request;
        request.set_room_id(roomId);
        request.set_guest_id(guestId);
        request.set_check_in_unix(std::chrono::system_clock::to_time_t(checkIn));
        request.set_check_out_unix(std::chrono::system_clock::to_time_t(checkOut));

        hotel::CreateBookingResponse response;
        auto status = stub_->CreateBooking(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        if (!response.success()) {
            std::cout << response.message() << "\n";
            return;
        }
        std::cout << "Booking created with ID " << response.booking_id() << " (state: New)\n";
    }

    void changeState(const std::string& action, const std::string& traceId) {
        int id;
        std::cout << "Booking ID: "; std::cin >> id;

        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::ChangeStateRequest request;
        request.set_booking_id(id);
        request.set_action(action);

        hotel::ChangeStateResponse response;
        auto status = stub_->ChangeBookingState(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        if (!response.success()) {
            std::cout << response.message() << "\n";
            return;
        }
        std::cout << "New state: " << response.new_state() << "\n";
    }

    void calculateCost(int strategy, const std::string& traceId) {
        int id;
        std::cout << "Booking ID: "; std::cin >> id;

        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::CalculateCostRequest request;
        request.set_booking_id(id);
        request.set_strategy_type(strategy);

        hotel::CalculateCostResponse response;
        auto status = stub_->CalculateCost(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        if (!response.success()) {
            std::cout << response.message() << "\n";
            return;
        }
        std::cout << "Total cost: $" << response.cost() << "\n";
    }

    int chooseStrategy() {
        int strat;
        std::cout << "1. Standard\n2. Loyalty discount (10% off)\n3. Holiday (+30%)\nChoice: ";
        std::cin >> strat;
        if (strat == 2) return 2;
        if (strat == 3) return 3;
        return 1;
    }

    void showFullReport(const std::string& traceId) {
        int mode;
        std::cout << "Revenue mode: 1. Standard  2. Loyalty (-10%)  3. Holiday (+30%)\nChoice: ";
        std::cin >> mode;

        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::ReportRequest request;
        request.set_revenue_mode(mode);

        hotel::ReportResponse response;
        auto status = stub_->GetFullReport(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        if (!response.message().empty()) {
            std::cout << response.message() << "\n";
        }
        std::cout << response.report_text();
    }

    void sendAdminNotification(const std::string& traceId) {
        int channel;
        std::string message;
        std::cout << "Channel: 1. Email  2. SMS  3. Console\nChoice: ";
        std::cin >> channel;
        std::cout << "Message: ";
        std::cin.ignore();
        std::getline(std::cin, message);

        grpc::ClientContext context;
        trace_util::addToClientContext(context, traceId);
        hotel::NotifyRequest request;
        request.set_channel(channel);
        request.set_message(message);

        hotel::NotifyResponse response;
        auto status = stub_->NotifyAdmin(&context, request, &response);
        if (!status.ok()) {
            std::cout << "Error: " << status.error_message() << "\n";
            return;
        }
        std::cout << response.notification_text() << "\n";
    }

    std::unique_ptr<hotel::CoreService::Stub> stub_;
};

#endif
