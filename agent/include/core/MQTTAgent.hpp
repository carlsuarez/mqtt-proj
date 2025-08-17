#ifndef MQTTAGENT_HPP
#define MQTTAGENT_HPP

#include "Config.hpp"
#include "MQTTCallback.hpp"
#include <atomic>
#include <cstdlib>
#include <ctime>
#include <mqtt/async_client.h>
#include <mqtt/delivery_token.h>
#include <mqtt/message.h>
#include <mqtt/reason_code.h>

/*
 * Basic MQTT Connection and Messaging
 */
class MQTTAgent {
private:
  static MQTTAgent *instance;

  // Object that describes the configuration of this client
  Config config_;

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
   * @param callback MQTTCallback object that determines the callback methods
   */
  static MQTTAgent &get_instance(const Config &config, MQTTCallback &callback);

  static void release_instance();

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
