#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

// Базовое исключение системы
class HotelSystemException : public std::runtime_error {
public:
    explicit HotelSystemException(const std::string& msg) : std::runtime_error(msg) {}
};

// Ошибки работы с номерами
class RoomException : public HotelSystemException {
public:
    explicit RoomException(const std::string& msg) : HotelSystemException(msg) {}
};

// Ошибки бронирования
class BookingException : public HotelSystemException {
public:
    explicit BookingException(const std::string& msg) : HotelSystemException(msg) {}
};

// Ошибки валидации данных
class ValidationException : public HotelSystemException {
public:
    explicit ValidationException(const std::string& msg) : HotelSystemException(msg) {}
};

// Ошибка: сущность не найдена
class NotFoundException : public HotelSystemException {
public:
    explicit NotFoundException(const std::string& msg) : HotelSystemException(msg) {}
};

#endif