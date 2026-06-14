#ifndef CORE_SERVICE_IMPL_H
#define CORE_SERVICE_IMPL_H

#include "BookingService.h"
#include "HotelReportManager.h"
#include "ReferenceClient.h"
#include "core.grpc.pb.h"
#include <memory>

class CoreServiceImpl final : public hotel::CoreService::Service {
public:
    CoreServiceImpl(std::shared_ptr<BookingService> bookingService,
                    std::shared_ptr<ReferenceClient> referenceClient,
                    std::shared_ptr<HotelReportManager> reportManager);

    grpc::Status ListRooms(grpc::ServerContext* context,
                           const hotel::Empty* request,
                           hotel::RoomList* response) override;

    grpc::Status ListGuests(grpc::ServerContext* context,
                            const hotel::Empty* request,
                            hotel::GuestList* response) override;

    grpc::Status CreateBooking(grpc::ServerContext* context,
                               const hotel::CreateBookingRequest* request,
                               hotel::CreateBookingResponse* response) override;

    grpc::Status ChangeBookingState(grpc::ServerContext* context,
                                    const hotel::ChangeStateRequest* request,
                                    hotel::ChangeStateResponse* response) override;

    grpc::Status CalculateCost(grpc::ServerContext* context,
                               const hotel::CalculateCostRequest* request,
                               hotel::CalculateCostResponse* response) override;

    grpc::Status GetFullReport(grpc::ServerContext* context,
                               const hotel::ReportRequest* request,
                               hotel::ReportResponse* response) override;

    grpc::Status NotifyAdmin(grpc::ServerContext* context,
                             const hotel::NotifyRequest* request,
                             hotel::NotifyResponse* response) override;

private:
    std::shared_ptr<BookingService> bookingService_;
    std::shared_ptr<ReferenceClient> referenceClient_;
    std::shared_ptr<HotelReportManager> reportManager_;
};

#endif
