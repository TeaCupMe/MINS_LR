#ifndef BOOKING_STATE_H
#define BOOKING_STATE_H

#include <iostream>
#include <string>
#include <memory>

class BookingContext; // forward declaration

// Базовый класс состояния
class BookingState {
public:
    virtual ~BookingState() = default;
    virtual void confirm(BookingContext& ctx) = 0;
    virtual void checkIn(BookingContext& ctx) = 0;
    virtual void checkOut(BookingContext& ctx) = 0;
    virtual void cancel(BookingContext& ctx) = 0;
    virtual std::string getName() const = 0;
};

// ---- Конкретные состояния (определены до BookingContext) ----
class NewState : public BookingState {
public:
    void confirm(BookingContext& ctx) override;
    void checkIn(BookingContext&) override { std::cout << "Cannot check in before confirmation.\n"; }
    void checkOut(BookingContext&) override { std::cout << "Cannot check out before check in.\n"; }
    void cancel(BookingContext& ctx) override;
    std::string getName() const override { return "New"; }
};

class ConfirmedState : public BookingState {
public:
    void confirm(BookingContext&) override { std::cout << "Already confirmed.\n"; }
    void checkIn(BookingContext& ctx) override;
    void checkOut(BookingContext&) override { std::cout << "Cannot check out before check in.\n"; }
    void cancel(BookingContext& ctx) override;
    std::string getName() const override { return "Confirmed"; }
};

class CheckedInState : public BookingState {
public:
    void confirm(BookingContext&) override { std::cout << "Already checked in.\n"; }
    void checkIn(BookingContext&) override { std::cout << "Already checked in.\n"; }
    void checkOut(BookingContext& ctx) override;
    void cancel(BookingContext&) override { std::cout << "Cannot cancel after check in.\n"; }
    std::string getName() const override { return "CheckedIn"; }
};

class CompletedState : public BookingState {
public:
    void confirm(BookingContext&) override { std::cout << "Already completed.\n"; }
    void checkIn(BookingContext&) override { std::cout << "Already completed.\n"; }
    void checkOut(BookingContext&) override { std::cout << "Already checked out.\n"; }
    void cancel(BookingContext&) override { std::cout << "Cannot cancel completed booking.\n"; }
    std::string getName() const override { return "Completed"; }
};

class CancelledState : public BookingState {
public:
    void confirm(BookingContext&) override { std::cout << "Cannot confirm cancelled booking.\n"; }
    void checkIn(BookingContext&) override { std::cout << "Cannot check in cancelled booking.\n"; }
    void checkOut(BookingContext&) override { std::cout << "Cannot check out cancelled booking.\n"; }
    void cancel(BookingContext&) override { std::cout << "Already cancelled.\n"; }
    std::string getName() const override { return "Cancelled"; }
};

// ---- Контекст, управляющий состоянием ----
class BookingContext {
public:
    BookingContext(int bookingId) : bookingId(bookingId) {
        currentState = std::make_shared<NewState>(); // теперь NewState известен
    }
    void setState(std::shared_ptr<BookingState> newState) { currentState = newState; }
    void confirm() { currentState->confirm(*this); }
    void checkIn() { currentState->checkIn(*this); }
    void checkOut() { currentState->checkOut(*this); }
    void cancel() { currentState->cancel(*this); }
    int getBookingId() const { return bookingId; }
    std::string getStateName() const { return currentState->getName(); }
private:
    int bookingId;
    std::shared_ptr<BookingState> currentState;
};

// ---- Реализации методов, которые используют BookingContext ----
inline void NewState::confirm(BookingContext& ctx) {
    std::cout << "Booking " << ctx.getBookingId() << " confirmed.\n";
    ctx.setState(std::make_shared<ConfirmedState>());
}
inline void NewState::cancel(BookingContext& ctx) {
    std::cout << "Booking " << ctx.getBookingId() << " cancelled.\n";
    ctx.setState(std::make_shared<CancelledState>());
}
inline void ConfirmedState::checkIn(BookingContext& ctx) {
    std::cout << "Checked in for booking " << ctx.getBookingId() << ".\n";
    ctx.setState(std::make_shared<CheckedInState>());
}
inline void ConfirmedState::cancel(BookingContext& ctx) {
    std::cout << "Booking " << ctx.getBookingId() << " cancelled.\n";
    ctx.setState(std::make_shared<CancelledState>());
}
inline void CheckedInState::checkOut(BookingContext& ctx) {
    std::cout << "Checked out from booking " << ctx.getBookingId() << ".\n";
    ctx.setState(std::make_shared<CompletedState>());
}

#endif