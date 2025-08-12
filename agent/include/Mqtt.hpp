#pragma once
#include "CommandProcessor.hpp"
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "HeartbeatManager.hpp"
#include "Publisher.hpp"
#include "SubscriptionManager.hpp"

class Mqtt {
  /* Configuration file */
  Config config_;

  /* Maintainer for connection, disconnection and callbacks */
  ConnectionManager connection_;

  /* Maintainer for subscriptions */
  SubscriptionManager subscriptions_;

  /* Parses and processes commands recieved */
  CommandProcessor commands_;

  /* Maintainer for publishing */
  Publisher publisher_;

  /* Responsible for sending heartbeat */
  HeartbeatManager heartbeat_;

public:
  /*
   * Constructs Mqtt object given a config object
   * @param config Object that specifies configuration of the agent.
   */
  explicit Mqtt(Config config);

  /*
   * Starts the agent
   */
  void start();

  /*
   * Stops the agent
   */
  void stop();
};
