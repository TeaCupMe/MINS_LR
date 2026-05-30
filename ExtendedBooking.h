#ifndef EXTENDED_BOOKING_H
#define EXTENDED_BOOKING_H

#include "patterns/state/BookingState.h"
#include "patterns/strategy/PricingStrategy.h"
#include "patterns/observer/Observer.h"
#include "entities.h"
#include <chrono>
#include <memory>
#include <string>

class ExtendedBooking : public Observable {
public:
    ExtendedBooking(int id, int roomId, int guestId,
                    std::chrono::system_clock::time_point checkInTime,
                    std::chrono::system_clock::time_point checkOutTime,
                    double pricePerNight)
        : context(id), roomId(roomId), guestId(guestId),
          checkInTime(checkInTime), checkOutTime(checkOutTime), pricePerNight(pricePerNight)
    {
        attach(std::make_shared<ConsoleNotifier>());
        attach(std::make_shared<LoggerNotifier>());
    }

    void confirm() {
        std::string old = context.getStateName();
        context.confirm();
        notify(context.getBookingId(), old, context.getStateName());
    }
    void checkIn() {
        std::string old = context.getStateName();
        context.checkIn();
        notify(context.getBookingId(), old, context.getStateName());
    }
    void checkOut() {
        std::string old = context.getStateName();
        context.checkOut();
        notify(context.getBookingId(), old, context.getStateName());
    }
    void cancel() {
        std::string old = context.getStateName();
        context.cancel();
        notify(context.getBookingId(), old, context.getStateName());
    }

    double calculateTotalCost(const PricingStrategy& strategy) const {
        if (context.getStateName() == "Cancelled") return 0.0;
        long nights = std::chrono::duration_cast<std::chrono::hours>(checkOutTime - checkInTime).count() / 24;
        if (nights < 1) nights = 1;
        return strategy.calculate(pricePerNight, nights);
    }

    std::string getState() const { return context.getStateName(); }
    int getId() const { return context.getBookingId(); }
    int getRoomId() const { return roomId; }
    int getGuestId() const { return guestId; }

private:
    BookingContext context;
    int roomId;
    int guestId;
    std::chrono::system_clock::time_point checkInTime;
    std::chrono::system_clock::time_point checkOutTime;
    double pricePerNight;
};

#endif