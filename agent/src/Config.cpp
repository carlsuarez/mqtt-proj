#include "Config.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>

Config Config::loadFromFile(const std::string &path) {
  Config cfg;
  std::ifstream file(path);
  if (!file) {
    throw std::runtime_error("Failed to open config file: " + path);
  }
  json data = json::parse(file);
  std::cout << data.dump(4) << std::endl;
  cfg.device_id = data["device_id"];
  cfg.broker = data["mqtt"]["broker"];
  cfg.port = data["mqtt"]["port"];
  cfg.ca_file = data["mqtt"]["ca_file"];
  cfg.cert_file = data["mqtt"]["cert_file"];
  cfg.key_file = data["mqtt"]["key_file"];
  cfg.keepalive = data["mqtt"]["keepalive"];
  cfg.cmd_sub = data["topics"]["cmd_sub"];
  cfg.cmd_resp_pub = data["topics"]["cmd_resp_pub"];
  cfg.metrics_pub = data["topics"]["metrics_pub"];
  cfg.heartbeat_interval_s = data["heartbeat_interval_s"];
  return cfg;
}

Config::Config(const Config &obj)
    : device_id(obj.device_id), broker(obj.broker), port(obj.port),
      ca_file(obj.ca_file), cert_file(obj.cert_file), key_file(obj.key_file),
      keepalive(obj.keepalive), cmd_sub(obj.cmd_sub),
      cmd_resp_pub(obj.cmd_resp_pub), metrics_pub(obj.metrics_pub),
      heartbeat_interval_s(obj.heartbeat_interval_s) {}
