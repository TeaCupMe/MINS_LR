cmake_minimum_required(VERSION 3.16)
project(hotel_system CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PROTO_DIR ${CMAKE_SOURCE_DIR}/proto)
set(GENERATED_DIR ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GENERATED_DIR})

set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)
set(_GRPC_CPP_PLUGIN $<TARGET_FILE:gRPC::grpc_cpp_plugin>)

set(PROTO_GENERATED_SRCS
    ${GENERATED_DIR}/common.pb.cc
    ${GENERATED_DIR}/reference.pb.cc
    ${GENERATED_DIR}/reference.grpc.pb.cc
    ${GENERATED_DIR}/core.pb.cc
    ${GENERATED_DIR}/core.grpc.pb.cc
)

add_custom_command(
    OUTPUT ${GENERATED_DIR}/common.pb.cc ${GENERATED_DIR}/common.pb.h
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --cpp_out=${GENERATED_DIR} -I${PROTO_DIR} ${PROTO_DIR}/common.proto
    DEPENDS ${PROTO_DIR}/common.proto
    COMMENT "Generating common.pb"
)

add_custom_command(
    OUTPUT ${GENERATED_DIR}/reference.pb.cc ${GENERATED_DIR}/reference.pb.h
           ${GENERATED_DIR}/reference.grpc.pb.cc ${GENERATED_DIR}/reference.grpc.pb.h
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out=${GENERATED_DIR}
         --cpp_out=${GENERATED_DIR}
         --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN}
         -I${PROTO_DIR} ${PROTO_DIR}/reference.proto
    DEPENDS ${PROTO_DIR}/reference.proto ${PROTO_DIR}/common.proto
           ${GENERATED_DIR}/common.pb.h
    COMMENT "Generating reference.grpc.pb"
)

add_custom_command(
    OUTPUT ${GENERATED_DIR}/core.pb.cc ${GENERATED_DIR}/core.pb.h
           ${GENERATED_DIR}/core.grpc.pb.cc ${GENERATED_DIR}/core.grpc.pb.h
    COMMAND ${_PROTOBUF_PROTOC}
    ARGS --grpc_out=${GENERATED_DIR}
         --cpp_out=${GENERATED_DIR}
         --plugin=protoc-gen-grpc=${_GRPC_CPP_PLUGIN}
         -I${PROTO_DIR} ${PROTO_DIR}/core.proto
    DEPENDS ${PROTO_DIR}/core.proto ${PROTO_DIR}/common.proto
           ${GENERATED_DIR}/common.pb.h
    COMMENT "Generating core.grpc.pb"
)

add_library(hotel_proto ${PROTO_GENERATED_SRCS})
target_include_directories(hotel_proto PUBLIC ${GENERATED_DIR})
target_link_libraries(hotel_proto PUBLIC protobuf::libprotobuf gRPC::grpc++)

# Service B - Reference
add_executable(service_b
    service_b/main.cpp
    service_b/ReferenceServiceImpl.cpp
)
target_include_directories(service_b PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/service_b
    ${CMAKE_SOURCE_DIR}/shared
)
target_link_libraries(service_b PRIVATE hotel_proto gRPC::grpc++)

# Service A - Core
add_executable(service_a
    service_a/main.cpp
    service_a/CoreServiceImpl.cpp
    service_a/ReferenceClient.cpp
)
target_include_directories(service_a PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/service_a
    ${CMAKE_SOURCE_DIR}/shared
)
target_link_libraries(service_a PRIVATE hotel_proto gRPC::grpc++)

# Client
add_executable(client
    client/main.cpp
)
target_include_directories(client PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/shared
)
target_link_libraries(client PRIVATE hotel_proto gRPC::grpc++)
