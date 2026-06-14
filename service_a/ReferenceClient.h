#ifndef REFERENCE_CLIENT_H
#define REFERENCE_CLIENT_H

#include "reference.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

struct RoomInfo {
    int id;
    std::string category;
    double pricePerNight;
    int maxGuests;
    bool available;
};

struct GuestInfo {
    int id;
    std::string name;
    std::string phone;
};

struct ValidateResult {
    bool serviceAvailable = false;
    bool valid = false;
    std::string errorMessage;
    double pricePerNight = 0.0;
};

class ReferenceClient {
public:
    explicit ReferenceClient(std::shared_ptr<grpc::Channel> channel);

    std::optional<std::vector<RoomInfo>> listRooms(const std::string& traceId);
    std::optional<RoomInfo> getRoom(int roomId, const std::string& traceId);
    std::optional<std::vector<GuestInfo>> listGuests(const std::string& traceId);
    std::optional<GuestInfo> getGuest(int guestId, const std::string& traceId);
    ValidateResult validateBooking(int roomId, int guestId, const std::string& traceId);
    bool setRoomAvailability(int roomId, bool available, const std::string& traceId);

    static bool isUnavailable(const grpc::Status& status);

private:
    std::unique_ptr<hotel::ReferenceService::Stub> stub_;
};

#endif
