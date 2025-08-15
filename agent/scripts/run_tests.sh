#!/bin/bash
set -e  # Exit on any error

echo "Starting Mosquitto broker in Docker..."
docker compose up -d

# Build and run tests
echo "Building and running tests..."
mkdir -p build
cd build
cmake ..
make
ctest --output-on-failure
cd ..

# Stop and remove Docker container
echo "Stopping Mosquitto broker..."
docker compose down

echo "All done!"
