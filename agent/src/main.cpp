#include "Config.hpp"
#include "Mqtt.hpp"
#include <cstring>
#include <string>

const std::string DEFAULT_CONFIG_PATH{"/etc/mqtt-agent/config.json"};

int main(int argc, char *argv[]) {
  // Read path to configuration file
  std::string config_path;

  if (argc >= 3 && std::strcmp(argv[1], "--config"))
    config_path = std::string(argv[2]);
  else
    config_path = DEFAULT_CONFIG_PATH;

  // Create the Mqtt object
  try {
    Mqtt agent(Config::loadFromFile(config_path));
    agent.start();
    agent.stop();
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
  }
}
