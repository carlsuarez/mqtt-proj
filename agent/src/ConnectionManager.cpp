#include "ConnectionManager.hpp"
#include <iostream>
/*
 * Example code used:
 * https://github.com/eclipse-paho/paho.mqtt.cpp/blob/master/examples/sync_publish.cpp
 */
ConnectionManager::ConnectionManager(const Config &config) {
  // Create the unique ptr for mqtt::client object
std::cout << "Creating client. Broker: " << config.broker << std::endl;
  client = std::make_unique<mqtt::client>(config.broker, config.device_id);

  // Set the callback object for client
  client->set_callback(cb);

  // Set connection options
  connOpts.set_keep_alive_interval(config.keepalive);
  connOpts.set_clean_session(true);
}

int ConnectionManager::connect() {
  try {
    std::cout << "Connecting..." << std::endl;
    client->connect(connOpts);
    std::cout << "...Ok" << std::endl;
  } catch (const mqtt::exception &exc) {
    std::cerr << exc.what() << std::endl;
    return 1;
  }
  return 0;
}

int ConnectionManager::disconnect() {
   try {
    std::cout << "Disconnecting..." << std::endl;
    client->connect(connOpts);
    std::cout << "...Ok" << std::endl;
  } catch (const mqtt::exception &exc) {
    std::cerr << exc.what() << std::endl;
    return 1;
  }
    return 0;
}   

void ConnectionManager::test() {
    const std::string TOPIC{"hello"};
    const char *msg = "Hello World";
    client->publish(mqtt::message(TOPIC, msg, 1, false));
}
