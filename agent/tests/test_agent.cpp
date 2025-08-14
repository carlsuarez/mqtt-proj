#include "Config.hpp"
#include "MQTTAgent.hpp"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <mqtt/async_client.h>
#include <string>
#include <thread>
#include <iostream>

const std::string BROKER{"tcp://localhost:1883"};

TEST_CASE("MQTTAgent can publish and subscribe via Mosquitto", "[mqtt]") {
  std::atomic<bool> message_received{false};
  
  try {
    // Subscriber using Paho async client
    mqtt::async_client subscriber(BROKER, "test-subscriber");
    
    class MyCallback : public virtual mqtt::callback {
    public:
      std::atomic<bool> &flag;
      MyCallback(std::atomic<bool> &f) : flag(f) {}
      void message_arrived(mqtt::const_message_ptr msg) override {
        std::cout << "Received message: " << msg->get_payload_str() << std::endl;
        if (msg->get_payload_str() == "Hello Test") {
          flag = true;
        }
      }
      void connection_lost(const std::string& cause) override {
        std::cout << "Connection lost: " << cause << std::endl;
      }
    };
    
    MyCallback cb(message_received);
    subscriber.set_callback(cb);
    
    // Add connection options with timeout
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);
    connOpts.set_connect_timeout(std::chrono::seconds(10));
    
    std::cout << "Connecting subscriber to broker..." << std::endl;
    subscriber.connect(connOpts)->wait();
    std::cout << "Subscriber connected successfully" << std::endl;
    
    subscriber.subscribe("test/topic", 1)->wait();
    std::cout << "Subscriber subscribed to test/topic" << std::endl;
    
    // Setup agent
    Config config = ConfigBuilder()
                        .set_broker_url(BROKER)
                        .set_client_id("test-agent")
                        .set_qos_level(QoSLevel::AT_LEAST_ONCE)
                        .build();
    
    MQTTAgent agent(config);
    std::cout << "Connecting agent to broker..." << std::endl;
    REQUIRE(agent.connect() == true);
    std::cout << "Agent connected successfully" << std::endl;
    
    // Publish a message from agent
    std::cout << "Publishing message..." << std::endl;
    agent.publish_message("test/topic", "Hello Test", QoSLevel::AT_LEAST_ONCE);
    
    // Wait briefly for delivery
    std::cout << "Waiting for message delivery..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Increased wait time
    
    // Verify that subscriber received the message
    REQUIRE(message_received == true);
    
    // Cleanup
    std::cout << "Cleaning up..." << std::endl;
    agent.shutdown();
    subscriber.disconnect()->wait();
    
  } catch (const mqtt::exception& exc) {
    std::cerr << "MQTT Exception: " << exc.what() << std::endl;
    std::cerr << "Error code: " << exc.get_reason_code() << std::endl;
    FAIL("MQTT connection failed: " + std::string(exc.what()));
  }
}
