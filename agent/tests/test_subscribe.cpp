#include "Config.hpp"
#include "MQTTAgent.hpp"
#include "tests.hpp"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <chrono>
#include <iostream>
#include <mqtt/async_client.h>
#include <mqtt/message.h>
#include <string>
#include <thread>

const std::string BROKER{"tcp://localhost:1883"};

TEST_CASE("MQTTAgent can subscribe via Mosquitto", "[mqtt]") {
  std::atomic<bool> message_received{false};
  
  try {
    // Setup agent with callback that will track received messages
    Config config = ConfigBuilder()
                        .set_broker_url(BROKER)
                        .set_client_id("test-agent")
                        .set_qos_level(QoSLevel::AT_LEAST_ONCE)
                        .add_subscription("test/topic", QoSLevel::AT_LEAST_ONCE)
                        .build();

    TestCallback agent_callback(message_received);
    MQTTAgent &agent = MQTTAgent::get_instance(config, agent_callback);
    
    std::cout << "Connecting agent to broker..." << std::endl;
    REQUIRE(agent.connect() == true);
    std::cout << "Agent connected successfully" << std::endl;

    // Give agent time to establish subscription
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Create publisher using simple callback (not MQTTCallback)
    mqtt::async_client publisher(BROKER, "test-publisher");
    
    // Use SimpleCallback for regular MQTT client
    SimpleCallback simple_cb(message_received); // This won't be used for message reception
    publisher.set_callback(simple_cb);

    // Add connection options with timeout
    mqtt::connect_options connOpts;
    connOpts.set_keep_alive_interval(20);
    connOpts.set_clean_session(true);
    connOpts.set_connect_timeout(std::chrono::seconds(10));

    std::cout << "Connecting publisher to broker..." << std::endl;
    publisher.connect(connOpts)->wait();
    std::cout << "Publisher connected successfully" << std::endl;

    // Now publish the message (after agent is subscribed)
    std::cout << "Publishing message..." << std::endl;
    publisher
        .publish(mqtt::message_ptr_builder()
                     .topic("test/topic")
                     .payload("Hello Test")
                     .qos(1)
                     .finalize())
        ->wait_for(TIMEOUT);
    std::cout << "Message published successfully" << std::endl;

    // Wait for agent to receive the message
    std::cout << "Waiting for message delivery..." << std::endl;
    int wait_ms = 0;
    while (!message_received && wait_ms < 3000) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        wait_ms += 50;
    }

    // Verify that agent received the message
    REQUIRE(message_received == true);

    // Cleanup
    std::cout << "Cleaning up..." << std::endl;
    publisher.disconnect()->wait();
    std::cout << "Publisher disconnected" << std::endl;
    
    agent.shutdown();
    MQTTAgent::release_instance();
    std::cout << "Agent shutdown complete" << std::endl;

  } catch (const mqtt::exception &exc) {
    std::cerr << "MQTT Exception: " << exc.what() << std::endl;
    std::cerr << "Error code: " << exc.get_reason_code() << std::endl;
    FAIL("MQTT connection failed: " + std::string(exc.what()));
  } catch (const std::exception &e) {
    std::cerr << "Standard exception: " << e.what() << std::endl;
    FAIL("Test failed with exception: " + std::string(e.what()));
  }
}
