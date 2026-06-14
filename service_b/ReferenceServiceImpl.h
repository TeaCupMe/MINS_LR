#ifndef REFERENCE_SERVICE_IMPL_H
#define REFERENCE_SERVICE_IMPL_H

#include "repositories.h"
#include "reference.grpc.pb.h"
#include <memory>

class ReferenceServiceImpl final : public hotel::ReferenceService::Service {
public:
    ReferenceServiceImpl(std::shared_ptr<RoomRepository> roomRepo,
                         std::shared_ptr<GuestRepository> guestRepo);

    grpc::Status ListRooms(grpc::ServerContext* context,
                           const hotel::Empty* request,
                           hotel::RoomList* response) override;

    grpc::Status GetRoom(grpc::ServerContext* context,
                         const hotel::GetRoomRequest* request,
                         hotel::Room* response) override;

    grpc::Status ListGuests(grpc::ServerContext* context,
                            const hotel::Empty* request,
                            hotel::GuestList* response) override;

    grpc::Status GetGuest(grpc::ServerContext* context,
                          const hotel::GetGuestRequest* request,
                          hotel::Guest* response) override;

    grpc::Status ValidateBooking(grpc::ServerContext* context,
                                 const hotel::ValidateBookingRequest* request,
                                 hotel::ValidateBookingResponse* response) override;

    grpc::Status SetRoomAvailability(grpc::ServerContext* context,
                                     const hotel::SetAvailabilityRequest* request,
                                     hotel::Empty* response) override;

private:
    static hotel::Room toProto(const Room& room);
    static hotel::Guest toProto(const Guest& guest);

    std::shared_ptr<RoomRepository> roomRepo_;
    std::shared_ptr<GuestRepository> guestRepo_;
};

#endif
