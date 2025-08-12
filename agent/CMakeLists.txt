cmake_minimum_required(VERSION 3.16)

project(mqtt-agent VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Include directories
include_directories(include)

# Find required packages
find_package(Threads REQUIRED)
find_package(nlohmann_json REQUIRED)

# Collect source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(mqtt-agent.bin ${SOURCES})

# Link libraries
target_link_libraries(mqtt-agent.bin 
    paho-mqttpp3
    paho-mqtt3c
    Threads::Threads
    nlohmann_json::nlohmann_json
)

# Set output directory
set_target_properties(mqtt-agent.bin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
