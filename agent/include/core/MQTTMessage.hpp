#pragma once

#include <string>
#include <chrono>
#include <vector>

/**
 * Enumeration for MQTT Quality of Service levels
 */
enum class QoSLevel : int {
    AT_MOST_ONCE = 0,   // Fire and forget
    AT_LEAST_ONCE = 1,  // Acknowledged delivery
    EXACTLY_ONCE = 2    // Guaranteed delivery
};

/**
 * Enumeration for connection events
 */
enum class ConnectionEvent {
    CONNECTED,
    DISCONNECTED,
    CONNECTION_LOST,
    RECONNECTING,
    RECONNECTED
};

/**
 * Structure representing an MQTT message
 */
struct MQTTMessage {
    std::string topic;
    std::vector<uint8_t> payload;
    QoSLevel qos_level;
    bool retained;
    std::chrono::system_clock::time_point timestamp;
    int message_id;
    
    // Constructors
    MQTTMessage() 
        : qos_level(QoSLevel::AT_MOST_ONCE), retained(false), message_id(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    MQTTMessage(const std::string& topic_, const std::string& payload_str, 
                QoSLevel qos = QoSLevel::AT_MOST_ONCE, bool retained_ = false)
        : topic(topic_), qos_level(qos), retained(retained_), message_id(0) {
        payload.assign(payload_str.begin(), payload_str.end());
        timestamp = std::chrono::system_clock::now();
    }
    
    MQTTMessage(const std::string& topic_, const std::vector<uint8_t>& payload_,
                QoSLevel qos = QoSLevel::AT_MOST_ONCE, bool retained_ = false)
        : topic(topic_), payload(payload_), qos_level(qos), retained(retained_), message_id(0) {
        timestamp = std::chrono::system_clock::now();
    }
    
    // Utility methods
    std::string get_payload_as_string() const {
        return std::string(payload.begin(), payload.end());
    }
    
    size_t get_payload_size() const {
        return payload.size();
    }
    
    bool is_empty() const {
        return payload.empty();
    }
    
    // Get age of message in milliseconds
    int64_t get_age_ms() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp);
        return duration.count();
    }
    
    // Comparison operators
    bool operator==(const MQTTMessage& other) const {
        return topic == other.topic && 
               payload == other.payload && 
               qos_level == other.qos_level && 
               retained == other.retained;
    }
    
    bool operator!=(const MQTTMessage& other) const {
        return !(*this == other);
    }
};

/**
 * Structure for platform metrics
 */
struct PlatformMetrics {
    size_t messages_received = 0;
    size_t messages_sent = 0;
    size_t messages_processed = 0;
    size_t connection_events = 0;
    double average_processing_time_ms = 0.0;
    bool is_connected = false;
    std::chrono::system_clock::time_point start_time;
    
    PlatformMetrics() {
        start_time = std::chrono::system_clock::now();
    }
    
    int64_t get_uptime_seconds() const {
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
        return duration.count();
    }
    
    double get_messages_per_second() const {
        auto uptime = get_uptime_seconds();
        return uptime > 0 ? static_cast<double>(messages_processed) / uptime : 0.0;
    }
};
