#include "Config.hpp"
#include "MQTTMetrics.hpp"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <mqtt/async_client.h>
#include <mutex>

/**
 * Simple callback class for MQTT events
 */
class MQTTCallback : public virtual mqtt::callback,
                     public virtual mqtt::iaction_listener {
private:
  // Unique client ID (from config_)
  std::string client_id;

  std::ofstream log_file;
  std::mutex log_mutex;
  LogLevel log_level;
  bool log_to_console;

  // Central logging function
  void log(LogLevel level, const std::string &msg);

public:
  /*
   * Create an implementation of functions to respond to async events
   * @param client_id String that identifies the client. Only used for logging
   * purposes
   */
  MQTTCallback(const std::string &client_id, const log_options &logOpts);

  // Metrics for the platform
  PlatformMetrics metrics;

  // Connection callbacks
  virtual void connected(const std::string &cause) override;

  virtual void connection_lost(const std::string &cause) override;

  // Message callback
  virtual void message_arrived(mqtt::const_message_ptr msg) override;

  // Action callbacks
  virtual void on_failure(const mqtt::token &tok) override;

  virtual void on_success(const mqtt::token &tok) override;

  virtual void delivery_complete(mqtt::delivery_token_ptr tok) override;
};
