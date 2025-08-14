#!/bin/bash
set -e  # Exit on any error

CONTAINER_NAME="mosquitto-test"
BROKER_PORT=1883

echo "Starting Mosquitto broker in Docker..."
docker run -d --name $CONTAINER_NAME --network host eclipse-mosquitto

echo "Waiting for broker to initialize..."
sleep 5

# Test if the broker is actually reachable
echo "Testing broker connectivity..."
timeout=30
while ! nc -z localhost $BROKER_PORT; do
    sleep 1
    timeout=$((timeout-1))
    if [ $timeout -eq 0 ]; then
        echo "Timeout waiting for broker to start"
        docker logs $CONTAINER_NAME
        exit 1
    fi
done
echo "Broker is ready!"

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
docker stop $CONTAINER_NAME
docker rm $CONTAINER_NAME

echo "All done!"
