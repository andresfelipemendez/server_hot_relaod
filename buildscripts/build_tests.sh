#!/bin/bash

# Ensure the script is being executed from the buildscripts directory
SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR"

# Create build directory if it doesn't exist
mkdir -p ../build

# Compile the tests
g++ -std=c++17 ../tests/tests.cpp -o ../build/tests

if [ $? -eq 0 ]; then
	echo "Tests build successful."
	echo "Running tests..."
	../build/tests
else
	echo "Tests build failed."
	exit 1
fi
