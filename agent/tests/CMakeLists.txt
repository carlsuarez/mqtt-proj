cmake_minimum_required(VERSION 3.10)

# Find Catch2
find_package(Catch2 REQUIRED)

# Add test executable
add_executable(tests
    main.cpp
    test_agent.cpp
)

# Link required libraries 
target_link_libraries(tests PRIVATE
    Catch2::Catch2WithMain
    mqtt_agent_lib
)

# Add test to CTest
add_test(NAME MQTTAgentTests COMMAND tests)
