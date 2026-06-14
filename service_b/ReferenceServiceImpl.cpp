#include "ReferenceServiceImpl.h"
#include "../shared/grpc_logging.h"
#include "../shared/trace.h"

ReferenceServiceImpl::ReferenceServiceImpl(std::shared_ptr<RoomRepository> roomRepo,
                                           std::shared_ptr<GuestRepository> guestRepo)
    : roomRepo_(roomRepo), guestRepo_(guestRepo) {}

hotel::Room ReferenceServiceImpl::toProto(const Room& room) {
    hotel::Room proto;
    proto.set_id(room.getId());
    proto.set_category(room.categoryToString());
    proto.set_price_per_night(room.getPricePerNight());
    proto.set_max_guests(room.getMaxGuests());
    proto.set_available(room.getAvailable());
    return proto;
}

hotel::Guest ReferenceServiceImpl::toProto(const Guest& guest) {
    hotel::Guest proto;
    proto.set_id(guest.getId());
    proto.set_name(guest.getName());
    proto.set_phone(guest.getPhone());
    return proto;
}

grpc::Status ReferenceServiceImpl::ListRooms(grpc::ServerContext* context,
                                             const hotel::Empty*,
                                             hotel::RoomList* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "ReferenceService::ListRooms");

    response->set_success(true);
    for (const auto& room : roomRepo_->getAll()) {
        *response->add_rooms() = toProto(room);
    }
    return grpc::Status::OK;
}

grpc::Status ReferenceServiceImpl::GetRoom(grpc::ServerContext* context,
                                           const hotel::GetRoomRequest* request,
                                           hotel::Room* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "ReferenceService::GetRoom id=" + std::to_string(request->room_id()));

    auto* room = roomRepo_->findById(request->room_id());
    if (!room) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Room not found");
    }
    *response = toProto(*room);
    return grpc::Status::OK;
}

grpc::Status ReferenceServiceImpl::ListGuests(grpc::ServerContext* context,
                                              const hotel::Empty*,
                                              hotel::GuestList* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "ReferenceService::ListGuests");

    response->set_success(true);
    for (const auto& guest : guestRepo_->getAll()) {
        *response->add_guests() = toProto(guest);
    }
    return grpc::Status::OK;
}

grpc::Status ReferenceServiceImpl::GetGuest(grpc::ServerContext* context,
                                            const hotel::GetGuestRequest* request,
                                            hotel::Guest* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "ReferenceService::GetGuest id=" + std::to_string(request->guest_id()));

    auto* guest = guestRepo_->findById(request->guest_id());
    if (!guest) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Guest not found");
    }
    *response = toProto(*guest);
    return grpc::Status::OK;
}

grpc::Status ReferenceServiceImpl::ValidateBooking(grpc::ServerContext* context,
                                                   const hotel::ValidateBookingRequest* request,
                                                   hotel::ValidateBookingResponse* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "ReferenceService::ValidateBooking room=" +
                                std::to_string(request->room_id()) +
                                " guest=" + std::to_string(request->guest_id()));

    auto* room = roomRepo_->findById(request->room_id());
    if (!room) {
        response->set_valid(false);
        response->set_error_message("Room not found");
        return grpc::Status::OK;
    }
    if (!room->getAvailable()) {
        response->set_valid(false);
        response->set_error_message("Room not available");
        return grpc::Status::OK;
    }
    auto* guest = guestRepo_->findById(request->guest_id());
    if (!guest) {
        response->set_valid(false);
        response->set_error_message("Guest not found");
        return grpc::Status::OK;
    }

    response->set_valid(true);
    response->set_price_per_night(room->getPricePerNight());
    grpc_log::info(traceId, "ReferenceService::ValidateBooking -> OK");
    return grpc::Status::OK;
}

grpc::Status ReferenceServiceImpl::SetRoomAvailability(grpc::ServerContext* context,
                                                       const hotel::SetAvailabilityRequest* request,
                                                       hotel::Empty*) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "ReferenceService::SetRoomAvailability room=" +
                                std::to_string(request->room_id()) +
                                " available=" + (request->available() ? "true" : "false"));

    auto* room = roomRepo_->findById(request->room_id());
    if (!room) {
        return grpc::Status(grpc::StatusCode::NOT_FOUND, "Room not found");
    }
    room->setAvailable(request->available());
    return grpc::Status::OK;
}
