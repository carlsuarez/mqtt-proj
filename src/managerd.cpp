#include "mqtt/client.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <cstring>
#include <thread>
#include <chrono>
#include <string>

using json = nlohmann::json;

struct Topics
{
    std::string cmd_sub;
    std::string cmd_resp_pub;
    std::string metrics_pub;
};

struct Device
{
    int deviceID;
    std::string broker;
    unsigned short port;
    std::string ca_file;
    std::string cert_file;
    std::string key_file;
    int keepalive;
    Topics topics;
    int heartbeat_interval_s;
};

int main(int argc, char *argv[])
{

    //     mqtt::client client(serverURI, clientID);

    return 0;
}
