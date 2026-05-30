#ifndef OBSERVER_H
#define OBSERVER_H

#include <vector>
#include <memory>
#include <iostream>

class Observer {
public:
    virtual ~Observer() = default;
    virtual void update(int bookingId, const std::string& oldState, const std::string& newState) = 0;
};

class ConsoleNotifier : public Observer {
public:
    void update(int bookingId, const std::string& oldState, const std::string& newState) override {
        std::cout << "[NOTIFICATION] Booking #" << bookingId
                  << " state changed: " << oldState << " -> " << newState << std::endl;
    }
};

class LoggerNotifier : public Observer {
public:
    void update(int bookingId, const std::string& oldState, const std::string& newState) override {
        std::cout << "[LOG] Booking #" << bookingId
                  << ": " << oldState << " -> " << newState << std::endl;
    }
};

class Observable {
public:
    void attach(std::shared_ptr<Observer> observer) {
        observers.push_back(observer);
    }
    void notify(int id, const std::string& oldState, const std::string& newState) {
        for (auto& obs : observers)
            obs->update(id, oldState, newState);
    }
private:
    std::vector<std::shared_ptr<Observer>> observers;
};

#endif