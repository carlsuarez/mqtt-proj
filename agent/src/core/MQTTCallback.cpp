#include "MQTTCallback.hpp"

// Central logging function
void MQTTCallback::log(LogLevel level, const std::string &msg) {
  if (level < log_level)
    return;

  // Timestamp
  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream time_ss;
  time_ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

  std::ostringstream log_entry;
  log_entry << "[" << time_ss.str() << "]"
            << " [" << client_id << "]"
            << " [" << log_level_to_string(level) << "] " << msg;

  std::lock_guard<std::mutex> lock(log_mutex);

  // Console
  if (log_to_console)
    std::cout << log_entry.str() << std::endl;

  // File
  if (log_file.is_open())
    log_file << log_entry.str() << std::endl;
}

MQTTCallback::MQTTCallback(const std::string &client_id,
                           const log_options &logOpts)
    : client_id(client_id) {
  log_level = logOpts.log_level;

  if (!logOpts.log_file_path.empty())
    log_file.open(logOpts.log_file_path, std::ofstream::app);


  log_to_console = logOpts.log_to_console;
}
// Connection callbacks
void MQTTCallback::connected(const std::string &cause) {
  log(LogLevel::INFO, "Connected: " + cause);
}

void MQTTCallback::connection_lost(const std::string &cause) {
  log(LogLevel::ERROR, "Connection lost: " + cause);
}

// Message callback
void MQTTCallback::message_arrived(mqtt::const_message_ptr msg) {
  metrics.messages_received++;
  std::ostringstream ss;
  ss << "Message arrived | Topic: " << msg->get_topic()
     << " | Payload: " << msg->get_payload_str() << " | QoS: " << msg->get_qos()
     << " | Retained: " << (msg->is_retained() ? "true" : "false");
  log(LogLevel::INFO, ss.str());
}

// Action callbacks
void MQTTCallback::on_failure(const mqtt::token &tok) {
  if (tok.get_reason_code() == mqtt::SUCCESS) {
    on_success(tok);
    return;
  }
  log(LogLevel::ERROR,
      "Action failed: reason_code=" + std::to_string(tok.get_reason_code()));
}

void MQTTCallback::on_success(const mqtt::token &tok) {
  switch (tok.get_type()) {
  case mqtt::token::CONNECT:
    log(LogLevel::INFO, "Connection successful");
    break;
  case mqtt::token::SUBSCRIBE:
    log(LogLevel::INFO, "Subscription successful");
    break;
  case mqtt::token::PUBLISH:
    log(LogLevel::DEBUG, "Publish successful");
    break;
  default:
    log(LogLevel::DEBUG, "Action succeeded");
    break;
  }
}

void MQTTCallback::delivery_complete(mqtt::delivery_token_ptr tok) {
  metrics.messages_sent++;
  log(LogLevel::INFO, "Delivery complete for message ID: " +
                          std::to_string(tok->get_message_id()));
}
