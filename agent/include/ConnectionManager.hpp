#include "Config.hpp"
#include <memory>
#include <mqtt/client.h>
#include <mqtt/connect_options.h>

class ConnectionManager {

  /* Smart pointer to client object */
  std::unique_ptr<mqtt::client> client;

  /* Class to handler callbacks */
  class user_callback : public virtual mqtt::callback {
    /*
     * Callback that is triggered when client loses connection involuntarily
     * @param cause String that describes the cause of the connection loss
     */
    void connection_lost(const std::string &cause) override {
      std::cout << "\nConnection lost" << std::endl;
      if (!cause.empty())
        std::cout << "\tcause: " << cause << std::endl;
    }
    /*
     * Callback that is triggered when a delivery is complete
     * @param tok A pointer to a delivery_token object
     */
    void delivery_complete(mqtt::delivery_token_ptr tok) override {
      std::cout << "\n  [Delivery complete for token: "
                << (tok ? tok->get_message_id() : -1) << "]" << std::endl;
    }
  };
  user_callback cb;
  mqtt::connect_options connOpts;

public:
  /*
   * Create a connection manager given specifications from Config object
   * @param config A const referecnce to a Config object that is used for
   * important parameters in the construction of this ConnectionManager
   */
  ConnectionManager(const Config &config);

  /*
   * Connect to the broker that was specified
   * @return 0 if success, 1 on failure
   */
  int connect();

  /*
   * Disconnect from the broker that was specified
   * @return 0 if success, 1 on failure
   */
  int disconnect();

  /*
   * Test function. Sends "Hello World" message
   */
  void test();
};
