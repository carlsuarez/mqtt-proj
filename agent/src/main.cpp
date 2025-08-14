#include <chrono>
#include <csignal>
#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <thread>

// MQTT includes
#include <mqtt/async_client.h>

#include "Config.hpp"
#include "Interfaces.hpp"
#include "MQTTAgent.hpp"
#include "MQTTMessage.hpp"

const std::string DEFAULT_CONFIG_PATH{"config/default.json"};
/**
 * Main function - Entry point
 */
int main(int argc, char *argv[]) {
  // Setup signal handlers for graceful shutdown
  std::signal(SIGINT, MQTTAgent::signal_handler);
  std::signal(SIGTERM, MQTTAgent::signal_handler);

  std::string config_path = DEFAULT_CONFIG_PATH;
  if (argc >= 3 && std::strcmp(argv[2], "--config") == 0)
    config_path = argv[3];

  Config config = ConfigBuilder::load_from_json(config_path);

  try {
    MQTTAgent agent(config);
    agent.connect();
    agent.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
