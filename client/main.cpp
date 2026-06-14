#include "ConsoleUI.h"
#include "core.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>

int main() {
    auto channel = grpc::CreateChannel("localhost:50052", grpc::InsecureChannelCredentials());
    ConsoleUI ui(hotel::CoreService::NewStub(channel));
    std::cout << "Client connected to Core Service (localhost:50052)" << std::endl;
    ui.run();
    return 0;
}
