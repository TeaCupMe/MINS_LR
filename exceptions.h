#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

class HotelSystemException : public std::runtime_error {
public:
    explicit HotelSystemException(const std::string& msg) : std::runtime_error(msg) {}
};

class RoomException : public HotelSystemException {
public:
    explicit RoomException(const std::string& msg) : HotelSystemException(msg) {}
};

class BookingException : public HotelSystemException {
public:
    explicit BookingException(const std::string& msg) : HotelSystemException(msg) {}
};

class ValidationException : public HotelSystemException {
public:
    explicit ValidationException(const std::string& msg) : HotelSystemException(msg) {}
};

class NotFoundException : public HotelSystemException {
public:
    explicit NotFoundException(const std::string& msg) : HotelSystemException(msg) {}
};

#endif