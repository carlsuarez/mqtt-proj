cmake_minimum_required(VERSION 3.16)
project(MqttAgent CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable testing
enable_testing()

# Dependencies
find_package(PahoMqttCpp REQUIRED)
find_package(nlohmann_json REQUIRED)

# Create a library with the core functionality
set(LIB_SOURCES
    src/core/MQTTAgent.cpp
    src/core/Config.cpp
)

add_library(mqtt_agent_lib ${LIB_SOURCES})

# Include directories for the library
target_include_directories(mqtt_agent_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/core
)

# Link libraries to the core library
target_link_libraries(mqtt_agent_lib PUBLIC
    PahoMqttCpp::paho-mqttpp3
    nlohmann_json::nlohmann_json
)

# Create the main executable
add_executable(mqtt_agent src/main.cpp)

# Link the main executable to the library
target_link_libraries(mqtt_agent PRIVATE mqtt_agent_lib)

# Add tests subdirectory
add_subdirectory(tests)
