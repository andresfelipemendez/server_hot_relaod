#!/bin/bash

# Function to check if inotify-tools is installed
check_inotify_tools() {
	if ! command -v inotifywait &>/dev/null; then
		echo "inotify-tools is not installed."
		read -p "Do you want to install inotify-tools? (y/n) " choice
		case "$choice" in
		y | Y)
			echo "Installing inotify-tools..."
			sudo apt-get update
			sudo apt-get install -y inotify-tools
			;;
		*)
			echo "inotify-tools is required to run this script. Exiting."
			exit 1
			;;
		esac
	else
		echo "inotify-tools is already installed."
	fi
}

# Function to check if build-essential is installed
check_build_essential() {
    if ! dpkg -s build-essential &>/dev/null; then
        echo "build-essential is not installed."
        read -p "Do you want to install build-essential? (y/n) " choice
        case "$choice" in
        y | Y)
            echo "Installing build-essential..."
            sudo apt-get update
            sudo apt-get install -y build-essential
            ;;
        *)
            echo "build-essential is required to run this script. Exiting."
            exit 1
            ;;
        esac
    else
        echo "build-essential is already installed."
    fi
}

# Check and install inotify-tools if necessary
check_inotify_tools

# Check and install build-essential if necessary
check_build_essential

# Call the build_watcher.sh script
./buildscripts/build_watcher.sh

# Check if build_watcher.sh was successful
if [ $? -ne 0 ]; then
	echo "build_watcher.sh failed. Exiting."
	exit 1
fi

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
./build/hotreload
