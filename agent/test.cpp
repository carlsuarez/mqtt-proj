// test_connect.cpp
#include <iostream>
#include <mqtt/client.h>

int main() {
    try {
        mqtt::client c("tcp://10.0.0.14:1883", "test-client");
        mqtt::connect_options opts;
        opts.set_keep_alive_interval(20);
        opts.set_clean_session(true);
        std::cout << "About to connect\n";
        c.connect(opts);
        std::cout << "Connected OK\n";
        c.disconnect();
    } catch (const mqtt::exception &e) {
        std::cerr << "mqtt exception: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

