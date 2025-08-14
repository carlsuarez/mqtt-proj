#include "MQTTAgent.hpp"
#include <memory>
#include <mqtt/async_client.h>
#include <mqtt/create_options.h>

std::atomic<bool> MQTTAgent::shutdown_requested{false};

MQTTAgent::MQTTAgent(const Config &config) : config_(config) {
  // Create MQTT client
  if (config_.enable_persistence)
    client_ = std::make_unique<mqtt::async_client>(
        config_.broker_url, config_.client_id, config_.persistence_directory);
  else
    client_ = std::make_unique<mqtt::async_client>(
        config_.broker_url, config_.client_id, mqtt::NO_PERSISTENCE);

  // Create callback
  callback_ = std::make_unique<MQTTCallback>(config_.client_id);
  client_->set_callback(*callback_);

  // Setup connection options
  setup_connection_options();
}

bool MQTTAgent::connect() {
  try {
    std::cout << "Connecting to MQTT broker..." << std::endl;
    auto token = client_->connect(connect_options_, nullptr, *callback_);
    token->wait(); // This will be changed to a non-blocking operation

    if (!client_->is_connected()) {
      std::cerr << "Failed to connect to broker" << std::endl;
      return false;
    }

    // Subscribe to topics
    for (const auto &topic : config_.subscriptions) {
      auto qos =
          config_.subscription_qos.find(topic) != config_.subscription_qos.end()
              ? static_cast<int>(config_.subscription_qos.at(topic))
              : static_cast<int>(config_.qos_level);

      std::cout << "Subscribing to: " << topic << " (QoS " << qos << ")"
                << std::endl;
      client_->subscribe(topic, qos, nullptr, *callback_);
    }

    return true;

  } catch (const mqtt::exception &exc) {
    std::cerr << "MQTT Exception: " << exc.what() << std::endl;
    return false;
  }
}

void MQTTAgent::publish_message(const std::string &topic,
                                const std::string &payload, QoSLevel qos,
                                bool retained) {
  try {
    auto msg = mqtt::make_message(topic, payload);
    msg->set_qos(static_cast<int>(qos));
    msg->set_retained(retained);

    std::cout << "Publishing to " << topic << ": " << payload << std::endl;
    client_->publish(msg, nullptr, *callback_);

  } catch (const mqtt::exception &exc) {
    std::cerr << "Publish failed: " << exc.what() << std::endl;
  }
}

void MQTTAgent::run() {
  std::cout << "Platform running... Press Ctrl+C to stop" << std::endl;

  // Publish a startup message
  publish_message("platform/status", "Platform started",
                  QoSLevel::AT_LEAST_ONCE, true);

  // Main loop - in later phases this will be replaced with proper threading
  int message_count = 0;
  while (!shutdown_requested && client_->is_connected()) {
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Send periodic heartbeat
    publish_message("platform/heartbeat",
                    "Heartbeat " + std::to_string(++message_count));
  }

  // Publish shutdown message and shutdown
  if (client_->is_connected()) {
    publish_message("platform/status", "Platform shutting down",
                    QoSLevel::AT_LEAST_ONCE, true);
    shutdown();
  }
}

void MQTTAgent::shutdown() {
  std::cout << "Shutting down platform..." << std::endl;

  if (client_->is_connected()) {
    try {
      // Unsubscribe from all topics
      for (const auto &topic : config_.subscriptions) {
        client_->unsubscribe(topic);
      }

      // Disconnect
      auto token = client_->disconnect();
      token->wait(); // This will be changed to a non-blocking operation

    } catch (const mqtt::exception &exc) {
      std::cerr << "Shutdown error: " << exc.what() << std::endl;
    }
  }

  std::cout << "Platform shutdown complete." << std::endl;
}

void MQTTAgent::setup_connection_options() {
  mqtt::connect_options_builder builder;

  builder.clean_session(config_.clean_session)
      .keep_alive_interval(config_.keep_alive_interval)
      .connect_timeout(config_.connect_timeout)
      .automatic_reconnect(config_.automatic_reconnect);

  // Set credentials if provided
  if (!config_.username.empty()) {
    builder.user_name(config_.username);
    if (!config_.password.empty()) {
      builder.password(config_.password);
    }
  }

  // Set Last Will Testament if enabled
  if (config_.enable_last_will) {
    auto will_msg = mqtt::message(
        config_.last_will_topic, config_.last_will_message,
        static_cast<int>(config_.last_will_qos), config_.last_will_retained);
    builder.will(will_msg);
  }

  connect_options_ = builder.finalize();
}
