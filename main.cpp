#include "ui.h"
#include "repositories.h"
#include "services.h"

int main() {
    auto roomRepo = std::make_shared<RoomRepository>();
    auto guestRepo = std::make_shared<GuestRepository>();
    auto bookingRepo = std::make_shared<BookingRepository>();

    // Добавим тестовые данные
    roomRepo->add(Room(1, Room::Category::STANDARD, 100.0, 2));
    roomRepo->add(Room(2, Room::Category::APARTMENT, 250.0, 4));
    guestRepo->add(Guest(1, "Alice", "+123456789"));
    guestRepo->add(Guest(2, "Bob",   "+987654321"));

    auto bookingSvc = std::make_shared<BookingService>(roomRepo, guestRepo, bookingRepo);
    auto reportSvc  = std::make_shared<ReportService>(bookingRepo, roomRepo, guestRepo);

    ConsoleUI ui(bookingSvc, reportSvc, roomRepo, guestRepo);
    ui.run();

    return 0;
}