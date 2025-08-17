#pragma once

#include <chrono>
#include <atomic>

/**
 * Structure for platform metrics
 */
struct PlatformMetrics {
    std::atomic<size_t> messages_received = 0;
    std::atomic<size_t> messages_sent = 0;
    std::atomic<size_t> messages_processed = 0;
    std::atomic<size_t> connection_events = 0;
    std::atomic<double> average_processing_time_ms = 0.0;
    std::atomic<bool> is_connected = false;
    std::atomic<std::chrono::system_clock::time_point> start_time;
    
    PlatformMetrics() {
        start_time = std::chrono::system_clock::now();
    }
    
    int64_t get_uptime_seconds() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time.load());
        return duration.count();
    }
    
    double get_messages_per_second() const {
        auto uptime = get_uptime_seconds();
        return uptime > 0 ? static_cast<double>(messages_processed) / uptime : 0.0;
    }
};
