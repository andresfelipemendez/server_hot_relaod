#!/bin/bash

SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR"

mkdir -p ../build
gcc -o ../build/filewatcher ../src/watcher/filewatcher.c

if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable is located at build/filewatcher"
else
    echo "Compilation failed."
fi