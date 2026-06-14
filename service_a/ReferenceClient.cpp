#include "ReferenceClient.h"
#include "../shared/grpc_logging.h"
#include "../shared/trace.h"

ReferenceClient::ReferenceClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(hotel::ReferenceService::NewStub(channel)) {}

bool ReferenceClient::isUnavailable(const grpc::Status& status) {
    return !status.ok() &&
           (status.error_code() == grpc::StatusCode::UNAVAILABLE ||
            status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED);
}

std::optional<std::vector<RoomInfo>> ReferenceClient::listRooms(const std::string& traceId) {
    grpc_log::info(traceId, "ReferenceClient::ListRooms");
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
    trace_util::addToClientContext(context, traceId);

    hotel::Empty request;
    hotel::RoomList response;
    auto status = stub_->ListRooms(&context, request, &response);
    if (isUnavailable(status)) {
        grpc_log::error(traceId, "ReferenceClient::ListRooms -> service unavailable");
        return std::nullopt;
    }
    if (!status.ok()) return std::nullopt;

    std::vector<RoomInfo> rooms;
    for (const auto& r : response.rooms()) {
        rooms.push_back({r.id(), r.category(), r.price_per_night(), r.max_guests(), r.available()});
    }
    return rooms;
}

std::optional<RoomInfo> ReferenceClient::getRoom(int roomId, const std::string& traceId) {
    grpc_log::info(traceId, "ReferenceClient::GetRoom id=" + std::to_string(roomId));
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
    trace_util::addToClientContext(context, traceId);

    hotel::GetRoomRequest request;
    request.set_room_id(roomId);
    hotel::Room response;
    auto status = stub_->GetRoom(&context, request, &response);
    if (isUnavailable(status)) return std::nullopt;
    if (!status.ok()) return std::nullopt;

    return RoomInfo{response.id(), response.category(), response.price_per_night(),
                    response.max_guests(), response.available()};
}

std::optional<std::vector<GuestInfo>> ReferenceClient::listGuests(const std::string& traceId) {
    grpc_log::info(traceId, "ReferenceClient::ListGuests");
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
    trace_util::addToClientContext(context, traceId);

    hotel::Empty request;
    hotel::GuestList response;
    auto status = stub_->ListGuests(&context, request, &response);
    if (isUnavailable(status)) {
        grpc_log::error(traceId, "ReferenceClient::ListGuests -> service unavailable");
        return std::nullopt;
    }
    if (!status.ok()) return std::nullopt;

    std::vector<GuestInfo> guests;
    for (const auto& g : response.guests()) {
        guests.push_back({g.id(), g.name(), g.phone()});
    }
    return guests;
}

std::optional<GuestInfo> ReferenceClient::getGuest(int guestId, const std::string& traceId) {
    grpc_log::info(traceId, "ReferenceClient::GetGuest id=" + std::to_string(guestId));
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
    trace_util::addToClientContext(context, traceId);

    hotel::GetGuestRequest request;
    request.set_guest_id(guestId);
    hotel::Guest response;
    auto status = stub_->GetGuest(&context, request, &response);
    if (isUnavailable(status)) return std::nullopt;
    if (!status.ok()) return std::nullopt;

    return GuestInfo{response.id(), response.name(), response.phone()};
}

ValidateResult ReferenceClient::validateBooking(int roomId, int guestId, const std::string& traceId) {
    grpc_log::info(traceId, "ReferenceClient::ValidateBooking room=" + std::to_string(roomId) +
                                 " guest=" + std::to_string(guestId));
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
    trace_util::addToClientContext(context, traceId);

    hotel::ValidateBookingRequest request;
    request.set_room_id(roomId);
    request.set_guest_id(guestId);
    hotel::ValidateBookingResponse response;
    auto status = stub_->ValidateBooking(&context, request, &response);

    ValidateResult result;
    if (isUnavailable(status)) {
        grpc_log::error(traceId, "ReferenceClient::ValidateBooking -> service unavailable");
        result.serviceAvailable = false;
        result.errorMessage = "Reference service unavailable";
        return result;
    }
    if (!status.ok()) {
        result.serviceAvailable = false;
        result.errorMessage = status.error_message();
        return result;
    }

    result.serviceAvailable = true;
    result.valid = response.valid();
    result.errorMessage = response.error_message();
    result.pricePerNight = response.price_per_night();
    if (result.valid) {
        grpc_log::info(traceId, "ReferenceClient::ValidateBooking -> OK");
    }
    return result;
}

bool ReferenceClient::setRoomAvailability(int roomId, bool available, const std::string& traceId) {
    grpc_log::info(traceId, "ReferenceClient::SetRoomAvailability room=" + std::to_string(roomId));
    grpc::ClientContext context;
    context.set_deadline(std::chrono::system_clock::now() + std::chrono::seconds(3));
    trace_util::addToClientContext(context, traceId);

    hotel::SetAvailabilityRequest request;
    request.set_room_id(roomId);
    request.set_available(available);
    hotel::Empty response;
    auto status = stub_->SetRoomAvailability(&context, request, &response);
    if (isUnavailable(status)) {
        grpc_log::error(traceId, "ReferenceClient::SetRoomAvailability -> service unavailable");
        return false;
    }
    return status.ok();
}
