#!/bin/bash

set -e

docker compose up -d
mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
./build/mqtt_agent
docker compose down
