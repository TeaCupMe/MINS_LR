#include "CoreServiceImpl.h"
#include "../patterns/strategy/PricingStrategy.h"
#include "../shared/grpc_logging.h"
#include "../shared/trace.h"
#include <chrono>

CoreServiceImpl::CoreServiceImpl(std::shared_ptr<BookingService> bookingService,
                                   std::shared_ptr<ReferenceClient> referenceClient,
                                   std::shared_ptr<HotelReportManager> reportManager)
    : bookingService_(bookingService),
      referenceClient_(referenceClient),
      reportManager_(reportManager) {}

grpc::Status CoreServiceImpl::ListRooms(grpc::ServerContext* context,
                                        const hotel::Empty*,
                                        hotel::RoomList* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::ListRooms");

    auto rooms = referenceClient_->listRooms(traceId);
    if (!rooms) {
        response->set_success(false);
        response->set_message("Reference service unavailable. Please try again later.");
        return grpc::Status::OK;
    }

    response->set_success(true);
    for (const auto& room : *rooms) {
        auto* proto = response->add_rooms();
        proto->set_id(room.id);
        proto->set_category(room.category);
        proto->set_price_per_night(room.pricePerNight);
        proto->set_max_guests(room.maxGuests);
        proto->set_available(room.available);
    }
    return grpc::Status::OK;
}

grpc::Status CoreServiceImpl::ListGuests(grpc::ServerContext* context,
                                         const hotel::Empty*,
                                         hotel::GuestList* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::ListGuests");

    auto guests = referenceClient_->listGuests(traceId);
    if (!guests) {
        response->set_success(false);
        response->set_message("Reference service unavailable. Please try again later.");
        return grpc::Status::OK;
    }

    response->set_success(true);
    for (const auto& guest : *guests) {
        auto* proto = response->add_guests();
        proto->set_id(guest.id);
        proto->set_name(guest.name);
        proto->set_phone(guest.phone);
    }
    return grpc::Status::OK;
}

grpc::Status CoreServiceImpl::CreateBooking(grpc::ServerContext* context,
                                            const hotel::CreateBookingRequest* request,
                                            hotel::CreateBookingResponse* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::CreateBooking room=" + std::to_string(request->room_id()) +
                                " guest=" + std::to_string(request->guest_id()));

    auto checkIn = std::chrono::system_clock::from_time_t(request->check_in_unix());
    auto checkOut = std::chrono::system_clock::from_time_t(request->check_out_unix());

    auto result = bookingService_->createBooking(
        request->room_id(), request->guest_id(), checkIn, checkOut, traceId);

    response->set_success(result.success);
    response->set_message(result.message);
    if (result.success) {
        response->set_booking_id(result.bookingId);
    }
    return grpc::Status::OK;
}

grpc::Status CoreServiceImpl::ChangeBookingState(grpc::ServerContext* context,
                                                 const hotel::ChangeStateRequest* request,
                                                 hotel::ChangeStateResponse* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::ChangeBookingState id=" +
                                std::to_string(request->booking_id()) +
                                " action=" + request->action());

    auto booking = bookingService_->getExtendedBooking(request->booking_id());
    if (!booking) {
        response->set_success(false);
        response->set_message("Booking not found");
        return grpc::Status::OK;
    }

    const std::string& action = request->action();
    if (action == "confirm") booking->confirm();
    else if (action == "checkin") booking->checkIn();
    else if (action == "checkout") booking->checkOut();
    else if (action == "cancel") {
        booking->cancel();
        if (!bookingService_->releaseRoom(booking->getRoomId(), traceId)) {
            response->set_success(false);
            response->set_message("Reference service unavailable. Room availability not updated.");
            return grpc::Status::OK;
        }
    } else {
        response->set_success(false);
        response->set_message("Unknown action");
        return grpc::Status::OK;
    }

    response->set_success(true);
    response->set_message("State changed");
    response->set_new_state(booking->getState());
    return grpc::Status::OK;
}

grpc::Status CoreServiceImpl::CalculateCost(grpc::ServerContext* context,
                                            const hotel::CalculateCostRequest* request,
                                            hotel::CalculateCostResponse* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::CalculateCost id=" + std::to_string(request->booking_id()));

    auto booking = bookingService_->getExtendedBooking(request->booking_id());
    if (!booking) {
        response->set_success(false);
        response->set_message("Booking not found");
        return grpc::Status::OK;
    }

    std::unique_ptr<PricingStrategy> strategy;
    switch (request->strategy_type()) {
        case 2: strategy = std::make_unique<LoyaltyDiscountPricing>(); break;
        case 3: strategy = std::make_unique<HolidayPricing>(); break;
        default: strategy = std::make_unique<StandardPricing>(); break;
    }

    response->set_success(true);
    response->set_cost(booking->calculateTotalCost(*strategy));
    return grpc::Status::OK;
}

grpc::Status CoreServiceImpl::GetFullReport(grpc::ServerContext* context,
                                            const hotel::ReportRequest* request,
                                            hotel::ReportResponse* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::GetFullReport mode=" + std::to_string(request->revenue_mode()));

    auto result = reportManager_->buildFullReport(request->revenue_mode(), traceId);
    response->set_success(result.success);
    response->set_message(result.message);
    response->set_report_text(result.reportText);
    return grpc::Status::OK;
}

grpc::Status CoreServiceImpl::NotifyAdmin(grpc::ServerContext* context,
                                          const hotel::NotifyRequest* request,
                                          hotel::NotifyResponse* response) {
    std::string traceId = trace_util::fromServerContext(context);
    grpc_log::info(traceId, "CoreService::NotifyAdmin channel=" + std::to_string(request->channel()));

    std::string text = reportManager_->notifyAdmin(request->channel(), request->message());
    grpc_log::info(traceId, text);
    response->set_success(true);
    response->set_notification_text(text);
    return grpc::Status::OK;
}
