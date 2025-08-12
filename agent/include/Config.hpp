#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

struct Config {
  std::string device_id;
  std::string broker;
  unsigned short port;
  std::string ca_file;
  std::string cert_file;
  std::string key_file;
  int keepalive;
  std::string cmd_sub;
  std::string cmd_resp_pub;
  std::string metrics_pub;
  int heartbeat_interval_s;
  static Config loadFromFile(const std::string &file);
  Config() = default;
  Config(const Config &obj);
};
