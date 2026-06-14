#ifndef GRPC_LOGGING_H
#define GRPC_LOGGING_H

#include <iostream>
#include <string>

namespace grpc_log {

inline void info(const std::string& traceId, const std::string& message) {
    std::cout << "[trace_id=" << traceId << "] " << message << std::endl;
}

inline void error(const std::string& traceId, const std::string& message) {
    std::cerr << "[trace_id=" << traceId << "] ERROR: " << message << std::endl;
}

}  // namespace grpc_log

#endif
