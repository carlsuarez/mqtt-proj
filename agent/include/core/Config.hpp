#pragma once

#include "MQTTMessage.hpp"
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Configuration structure for the MQTT platform
 */
struct Config {
  // Connection settings
  std::string broker_url;
  std::string client_id;
  std::string username;
  std::string password;

  // Connection options
  std::chrono::seconds connect_timeout{10};
  std::chrono::seconds keep_alive_interval{60};
  bool clean_session = true;
  bool automatic_reconnect = false;
  std::chrono::seconds reconnect_delay;
  int max_reconnect_attempts = -1; // -1 = infinite

  // Threading settings
  size_t thread_pool_size = 4;
  size_t message_queue_size = 1000;

  // Persistence settings
  bool enable_persistence = false;
  std::string persistence_directory = "./persistence";

  // QoS settings
  QoSLevel qos_level = QoSLevel::AT_LEAST_ONCE;

  // Subscriptions
  std::vector<std::string> subscriptions;
  std::unordered_map<std::string, QoSLevel> subscription_qos;

  // Last Will Testament
  bool enable_last_will = false;
  std::string last_will_topic = "status/offline";
  std::string last_will_message = "Client disconnected unexpectedly";
  QoSLevel last_will_qos = QoSLevel::AT_LEAST_ONCE;
  bool last_will_retained = true;

  // SSL/TLS settings
  bool use_ssl = false;
  std::string ca_certificate_file;
  std::string client_certificate_file;
  std::string client_private_key_file;

  // Logging settings
  enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

  LogLevel log_level = LogLevel::INFO;
  std::string log_file_path;
  bool log_to_console = true;

  // Performance settings
  size_t max_inflight_messages = 20;
  std::chrono::milliseconds message_timeout{30000};

  // Metrics settings
  bool enable_metrics = true;
  std::chrono::seconds metrics_report_interval{60};

  // Validation method
  bool validate() const {
    if (broker_url.empty()) {
    std::
      cout << "No broker_url" << std::endl;
      return false;
    }
    if (client_id.empty()) {
    std::
      cout << "No client_id" << std::endl;
      return false;
    }
    if (thread_pool_size == 0) {
    std::
      cout << "No thread_pool_size " << std::endl;
      return false;
    }
    if (message_queue_size == 0) {
    std::
      cout << "No message_queue_size " << std::endl;
      return false;
    }
    if (enable_persistence && persistence_directory.empty()) {
    std::
      cout << "No enable_persistence " << std::endl;
      return false;
    }
    if (use_ssl && ca_certificate_file.empty()) {
    std::
      cout << "No use_ssl " << std::endl;
      return false;
    }
    return true;
  }
};

/**
 * Builder class for creating Config objects
 */
class ConfigBuilder {
private:
  Config config_;

public:
  ConfigBuilder() = default;

  /* SETTERS FOR BUILDER */
  ConfigBuilder &set_broker_url(const std::string &url);
  ConfigBuilder &set_client_id(const std::string &id);
  ConfigBuilder &set_credentials(const std::string &username,
                                 const std::string &password);
  ConfigBuilder &set_thread_pool_size(size_t count);
  ConfigBuilder &add_subscription(const std::string &topic, QoSLevel qos);
  ConfigBuilder &enable_persistence(const std::string &directory);
  ConfigBuilder &set_qos_level(QoSLevel qos);
  ConfigBuilder &enable_ssl(const std::string &ca_cert);
  ConfigBuilder &set_last_will(const std::string &topic,
                               const std::string &message, QoSLevel qos,
                               bool retained);

  ConfigBuilder &set_log_level(Config::LogLevel level);

  ConfigBuilder &enable_auto_reconnect(std::chrono::seconds delay);

  /*
   * @desc Build a Config object from a json file.
   * @param path Path to json file
   */
  static Config load_from_json(const std::string &path);

  /*
   * @desc Finalize the build of the Config object.
   */
  Config build() const;

private:
  /*
   * Helper function to convert a string to a Config::LogLevel
   * @param log_level String representing the log level. Valid strings are
   * "DEBUG", "INFO", "WARNING" and "ERROR"
   * @return Corresponding Config::LogLevel
   */
  static Config::LogLevel string_to_log_level(const std::string &log_level);
};
