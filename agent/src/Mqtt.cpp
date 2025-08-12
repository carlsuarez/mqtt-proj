#include "Mqtt.hpp"

Mqtt::Mqtt(Config config)
    : config_(std::move(config)),
      connection_(config_)
      //subscriptions_(connection_),
      //commands_(),
      //publisher_(connection_),
      //heartbeat_(publisher_, config.heartbeat_interval_s)
{}

void Mqtt::start()
{
    connection_.connect();
    connection_.test();
}

void Mqtt::stop()
{
    connection_.disconnect();
}
