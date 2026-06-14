#include "ReferenceServiceImpl.h"
#include "../entities.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>

void seedData(const std::shared_ptr<RoomRepository>& roomRepo,
              const std::shared_ptr<GuestRepository>& guestRepo) {
    roomRepo->add(Room(1, Room::Category::STANDARD, 100.0, 2));
    roomRepo->add(Room(2, Room::Category::APARTMENT, 250.0, 4));
    guestRepo->add(Guest(1, "Alice", "+123456789"));
    guestRepo->add(Guest(2, "Bob", "+987654321"));
}

int main() {
    auto roomRepo = std::make_shared<RoomRepository>();
    auto guestRepo = std::make_shared<GuestRepository>();
    seedData(roomRepo, guestRepo);

    ReferenceServiceImpl service(roomRepo, guestRepo);
    std::string address("0.0.0.0:50051");
    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Service B (Reference) listening on " << address << std::endl;
    server->Wait();
    return 0;
}
