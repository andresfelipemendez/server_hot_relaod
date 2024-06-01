#!/bin/bash

# Build the library (server)
echo "Building the library (server)..."
./buildscripts/build_lib.sh
if [ $? -ne 0 ]; then
    echo "Failed to build the library (server)."
    exit 1
fi

# Build the hot reload watcher
echo "Building the hot reload watcher..."
./buildscripts/build_hotreload.sh
if [ $? -ne 0 ]; then
    echo "Failed to build the hot reload watcher."
    exit 1
fi

# Run the hot reload watcher
echo "Running the hot reload watcher..."
./build/hotreload &

# Inform user that the server library is ready to be reloaded
echo "Server library is built and ready for hot reload."
