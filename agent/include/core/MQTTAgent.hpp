#ifndef MQTTAGENT_HPP
#define MQTTAGENT_HPP

#include "Config.hpp"
#include "MQTTMetrics.hpp"
#include <atomic>
#include <cstdlib>
#include <mqtt/async_client.h>
#include <mqtt/reason_code.h>

/**
 * Simple callback class for MQTT events
 */
class MQTTCallback : public virtual mqtt::callback,
                     public virtual mqtt::iaction_listener {
private:
  std::string client_id_;

public:
  MQTTCallback(const std::string &client_id) : client_id_(client_id) {}

  // Connection callbacks
  virtual void connected(const std::string &cause) override {
    std::cout << "[" << client_id_ << "] Connected: " << cause << std::endl;
  }

  virtual void connection_lost(const std::string &cause) override {
    std::cout << "[" << client_id_ << "] Connection lost: " << cause
              << std::endl;
  }

  // Message callback
  virtual void message_arrived(mqtt::const_message_ptr msg) override {
    std::cout << "[" << client_id_ << "] Message arrived:" << std::endl;
    std::cout << "  Topic: " << msg->get_topic() << std::endl;
    std::cout << "  Payload: " << msg->get_payload_str() << std::endl;
    std::cout << "  QoS: " << msg->get_qos() << std::endl;
    std::cout << "  Retained: " << (msg->is_retained() ? "true" : "false")
              << std::endl;
  }

  // Action callbacks
  virtual void on_failure(const mqtt::token &tok) override {
    if (tok.get_reason_code() == mqtt::SUCCESS)
      return on_success(tok); // Token completed asynchronously

    std::cout << "[" << client_id_
              << "] Action failed: " << tok.get_reason_code() << std::endl;
  }

  virtual void on_success(const mqtt::token &tok) override {
    if (tok.get_type() == mqtt::token::CONNECT) {
      std::cout << "[" << client_id_ << "] Connection successful" << std::endl;
    } else if (tok.get_type() == mqtt::token::SUBSCRIBE) {
      std::cout << "[" << client_id_ << "] Subscription successful"
                << std::endl;
    } else if (tok.get_type() == mqtt::token::PUBLISH) {
      std::cout << "[" << client_id_ << "] Publish successful" << std::endl;
    }
  }

  virtual void delivery_complete(mqtt::delivery_token_ptr tok) override {
    std::cout << "[" << client_id_
              << "] Delivery complete for message ID: " << tok->get_message_id()
              << std::endl;
  }
};

/*
 * Basic MQTT Connection and Messaging
 */
class MQTTAgent {
private:
  static MQTTAgent *instance;

  // Object that describes the configuration of this client
  Config config_;

  PlatformMetrics metrics;

  // Smart pointer to mqtt::async_client object
  std::unique_ptr<mqtt::async_client> client_;

  // Reference to a callback object
  MQTTCallback &callback_;

  // Connection options
  mqtt::connect_options connect_options_;

  // True if the user requests a shutdown via Ctrl + C
  static std::atomic<bool> shutdown_requested;

  /*
   * Constructor that copies config into member variable, creates
   * mqtt::async_client object and sets callback and connection options for the
   * client object
   * @param config Config object that describes the configuration for the agent
   */
  MQTTAgent(const Config &config, MQTTCallback &callback);

  /* Do not allow copying */
  MQTTAgent(const MQTTAgent &obj) = delete;

  /* Keep destructor private */
  ~MQTTAgent() = default;

public:
  /*
   * Get singleton instance of MQTTAgent.
   * @param config Config object that describes the configuration for the agent
   */
  static MQTTAgent &get_instance(const Config &config, MQTTCallback &callback);

  static void release_instance(void);
  /*
   * Establishes connection and subscribes to topics that were specified in
   * config
   */
  bool connect();

  /*
   *  Publishes a message via mqtt::async_client::publish.
   *  @param topic The topic for the message
   *  @param payload The payload for the message
   *  @param qos The QoS level for this message
   *  @param retained Determines whether this message is retained
   */
  void publish_message(const std::string &topic, const std::string &payload,
                       QoSLevel qos = QoSLevel::AT_LEAST_ONCE,
                       bool retained = false);

  /*
   * Starts the agent. Currently sends a heartbeat every 10 seconds until
   * interrupted by Ctrl + C.
   */
  void run();

  /*
   * Unsubscribes from all active subscriptions and disconnects the client.
   */
  void shutdown();

  /*
   * Function to be pointed to by std::signal. Turns shutdown_requested true
   * which causes a graceful shutdown
   */
  static void signal_handler(int signum) {
    std::cout << "\nReceived signal " << signum
              << ". Initiating graceful shutdown..." << std::endl;
    shutdown_requested = true;
  }

private:
  /*
   * Configures the connect_options member variable via the
   * mqtt::connect_options_builder.
   */
  void setup_connection_options();
};

#endif
