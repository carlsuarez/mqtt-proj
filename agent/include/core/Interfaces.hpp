#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

// Forward declarations
struct MQTTMessage;
enum class ConnectionEvent;

/**
 * Interface for handling MQTT messages
 */
class IMessageHandler {
public:
    virtual ~IMessageHandler() = default;
    
    /**
     * Handle an incoming MQTT message
     * @param message The MQTT message to handle
     */
    virtual void handle_message(const MQTTMessage& message) = 0;
    
    /**
     * Check if this handler can process messages from the given topic
     * @param topic The topic to check
     * @return true if this handler can process the topic
     */
    virtual bool can_handle(const std::string& topic) const = 0;
    
    /**
     * Get a description of what this handler does
     */
    virtual std::string get_description() const = 0;
};

/**
 * Observer interface for connection events
 */
class IConnectionObserver {
public:
    virtual ~IConnectionObserver() = default;
    
    virtual void on_connected() = 0;
    virtual void on_disconnected() = 0;
    virtual void on_connection_lost() = 0;
    virtual void on_reconnecting() = 0;
};

/**
 * Strategy interface for message routing
 */
class IRoutingStrategy {
public:
    virtual ~IRoutingStrategy() = default;
    
    /**
     * Get all handlers that should process this topic
     * @param topic The topic to route
     * @return Vector of handlers that can process this topic
     */
    virtual std::vector<std::shared_ptr<IMessageHandler>> 
        get_handlers(const std::string& topic) = 0;
    
    /**
     * Register a handler for a topic pattern
     * @param topic_pattern The topic pattern (can include wildcards)
     * @param handler The handler to register
     */
    virtual void register_handler(const std::string& topic_pattern,
                                std::shared_ptr<IMessageHandler> handler) = 0;
};

/**
 * Interface for command pattern implementation
 */
class ICommand {
public:
    virtual ~ICommand() = default;
    
    /**
     * Execute the command
     */
    virtual void execute() = 0;
    
    /**
     * Undo the command (if supported)
     */
    virtual void undo() = 0;
    
    /**
     * Check if this command supports undo
     */
    virtual bool can_undo() const = 0;
    
    /**
     * Get command description
     */
    virtual std::string get_description() const = 0;
};

/**
 * Interface for metrics collection
 */
class IMetricsCollector {
public:
    virtual ~IMetricsCollector() = default;
    
    virtual void increment_messages_received() = 0;
    virtual void increment_messages_sent() = 0;
    virtual void record_processing_time(double milliseconds) = 0;
    virtual void record_connection_event(ConnectionEvent event) = 0;
    
    virtual size_t get_messages_received() const = 0;
    virtual size_t get_messages_sent() const = 0;
    virtual double get_average_processing_time() const = 0;
};

/**
 * Callback type for async operations
 */
using AsyncCallback = std::function<void(bool success, const std::string& error_msg)>;
