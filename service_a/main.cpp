#include "CoreServiceImpl.h"
#include "BookingService.h"
#include "HotelReportManager.h"
#include "ReferenceClient.h"
#include "repositories.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>

int main() {
    auto referenceChannel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    auto referenceClient = std::make_shared<ReferenceClient>(referenceChannel);

    auto bookingRepo = std::make_shared<BookingRepository>();
    auto bookingService = std::make_shared<BookingService>(referenceClient, bookingRepo);
    auto reportManager = std::make_shared<HotelReportManager>(
        bookingRepo, referenceClient, bookingService);

    CoreServiceImpl service(bookingService, referenceClient, reportManager);
    std::string address("0.0.0.0:50052");
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Service A (Core) listening on " << address
              << " (Reference: localhost:50051)" << std::endl;
    server->Wait();
    return 0;
}
