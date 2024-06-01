#!/bin/bash

# Ensure the script is being executed from the buildscripts directory
SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR"

# Build the server.cpp to a shared library
g++ -shared -fPIC -o ../build/server_lib.so ../src/server/server.cpp
