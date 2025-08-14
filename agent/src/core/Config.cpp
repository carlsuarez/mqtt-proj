#include "Config.hpp"

ConfigBuilder &ConfigBuilder::set_broker_url(const std::string &url) {
  config_.broker_url = url;
  return *this;
}

ConfigBuilder &ConfigBuilder::set_client_id(const std::string &id) {
  config_.client_id = id;
  return *this;
}

ConfigBuilder &ConfigBuilder::set_credentials(const std::string &username,
                                              const std::string &password) {
  config_.username = username;
  config_.password = password;
  return *this;
}

ConfigBuilder &ConfigBuilder::set_thread_pool_size(size_t count) {
  config_.thread_pool_size = count;
  return *this;
}

ConfigBuilder &
ConfigBuilder::add_subscription(const std::string &topic,
                                QoSLevel qos = QoSLevel::AT_LEAST_ONCE) {
  config_.subscriptions.push_back(topic);
  config_.subscription_qos[topic] = qos;
  return *this;
}

ConfigBuilder &ConfigBuilder::enable_persistence(const std::string &directory) {
  config_.enable_persistence = true;
  config_.persistence_directory = directory;
  return *this;
}

ConfigBuilder &ConfigBuilder::set_qos_level(QoSLevel qos) {
  config_.qos_level = qos;
  return *this;
}

ConfigBuilder &ConfigBuilder::enable_ssl(const std::string &ca_cert = "") {
  config_.use_ssl = true;
  if (!ca_cert.empty()) {
    config_.ca_certificate_file = ca_cert;
  }
  return *this;
}

ConfigBuilder &ConfigBuilder::set_last_will(
    const std::string &topic, const std::string &message,
    QoSLevel qos = QoSLevel::AT_LEAST_ONCE, bool retained = true) {
  config_.enable_last_will = true;
  config_.last_will_topic = topic;
  config_.last_will_message = message;
  config_.last_will_qos = qos;
  config_.last_will_retained = retained;
  return *this;
}

ConfigBuilder &ConfigBuilder::set_log_level(Config::LogLevel level) {
  config_.log_level = level;
  return *this;
}

ConfigBuilder &ConfigBuilder::enable_auto_reconnect(
    std::chrono::seconds delay = std::chrono::seconds{5}) {
  config_.automatic_reconnect = true;
  config_.reconnect_delay = delay;
  return *this;
}

Config ConfigBuilder::load_from_json(const std::string &path) {
  std::ifstream file(path);
  if (!file.is_open())
    throw std::runtime_error("Couldn't open file");

  nlohmann::json j = nlohmann::json::parse(file);

  ConfigBuilder builder;

  if (j.contains("broker_url"))
    builder.set_broker_url(j["broker_url"]);

  if (j.contains("client_id"))
    builder.set_client_id(j["client_id"]);

  if (j.contains("qos_level"))
    builder.set_qos_level(j["qos_level"]);

  if (j.value("enable_persistence", false))
    builder.enable_persistence(
        j.value("persistence_directory", "./persistence"));

  if (j.contains("subscriptions") && j["subscriptions"].is_array())
    for (const auto &sub : j["subscriptions"])
      builder.add_subscription(
          sub["topic"], static_cast<QoSLevel>(
                            sub.value("qos_level", QoSLevel::AT_LEAST_ONCE)));

  if (j.contains("last_will") && j["last_will"].is_object()) {
    auto &last_will = j["last_will"];
    builder.set_last_will(last_will["topic"], last_will["message"],
                          static_cast<QoSLevel>(last_will.value(
                              "qos_level", QoSLevel::AT_LEAST_ONCE)),
                          last_will.value("retained", true));
  }

  if (j.value("use_ssl", false))
    builder.enable_ssl(
        j.value("ca_certificate_file", "./authenication/ca_certificate"));

  if (j.contains("log_level"))
    builder.set_log_level(string_to_log_level(j["log_level"]));

  if (j.value("enable_auto_reconnect", false))
    builder.enable_auto_reconnect(
        std::chrono::seconds(j.value("reconnect_delay", 5)));

  return builder.build();
}

Config ConfigBuilder::build() const {
  if (!config_.validate()) {
    throw std::invalid_argument("Invalid configuration");
  }
  return config_;
}

Config::LogLevel
ConfigBuilder::string_to_log_level(const std::string &log_level) {
  static const std::unordered_map<std::string, Config::LogLevel> map = {
      {"DEBUG", Config::LogLevel::DEBUG},
      {"INFO", Config::LogLevel::INFO},
      {"WARNING", Config::LogLevel::WARNING},
      {"ERROR", Config::LogLevel::ERROR}};

  std::string upper = log_level;
  std::transform(upper.begin(), upper.end(), upper.begin(), ::toupper);

  auto it = map.find(upper);
  if (it != map.end()) {
    return it->second;
  }
  throw std::invalid_argument("Invalid log level: " + log_level);
}
