#pragma once
#include <atomic>
#include <iostream>
#include <mqtt/async_client.h>
#include "MQTTAgent.hpp"

const std::chrono::seconds TIMEOUT{10};

// Callback for testing message reception
class TestCallback : public MQTTCallback {
public:
  std::atomic<bool> &flag;
  
  TestCallback(std::atomic<bool> &f) : MQTTCallback("test-callback"), flag(f) {}
  
  void message_arrived(mqtt::const_message_ptr msg) override {
    std::cout << "Received message: " << msg->get_payload_str() << std::endl;
    if (msg->get_payload_str() == "Hello Test") {
      flag = true;
    }
  }
};

// Dummy callback for agent when we don't need message handling
class DummyCallback : public MQTTCallback {
public:
  DummyCallback() : MQTTCallback("dummy-callback") {}
};

// Simple callback for external MQTT clients (not inheriting from MQTTCallback)
class SimpleCallback : public virtual mqtt::callback {
public:
  std::atomic<bool> &flag;
  
  SimpleCallback(std::atomic<bool> &f) : flag(f) {}
  
  void message_arrived(mqtt::const_message_ptr msg) override {
    std::cout << "Received message: " << msg->get_payload_str() << std::endl;
    if (msg->get_payload_str() == "Hello Test") {
      flag = true;
    }
  }
  
  void connection_lost(const std::string &cause) override {
    std::cout << "Connection lost: " << cause << std::endl;
  }
};
