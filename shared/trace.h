#ifndef TRACE_H
#define TRACE_H

#include <chrono>
#include <grpcpp/grpcpp.h>
#include <random>
#include <sstream>
#include <string>

namespace trace_util {

inline const char* kTraceIdKey = "x-trace-id";

inline std::string generateTraceId() {
    static std::mt19937 rng(static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<int> dist(0, 15);
    const char* hex = "0123456789abcdef";
    std::string id;
    id.reserve(16);
    for (int i = 0; i < 16; ++i) {
        id += hex[dist(rng)];
    }
    return id;
}

inline std::string fromServerContext(grpc::ServerContext* context) {
    auto it = context->client_metadata().find(kTraceIdKey);
    if (it != context->client_metadata().end()) {
        return std::string(it->second.data(), it->second.size());
    }
    return "no-trace";
}

inline void addToClientContext(grpc::ClientContext& context, const std::string& traceId) {
    context.AddMetadata(kTraceIdKey, traceId);
}

}  // namespace trace_util

#endif
