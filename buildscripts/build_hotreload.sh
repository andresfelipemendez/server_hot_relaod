#!/bin/bash

SCRIPT_DIR="$(dirname "$0")"
cd "$SCRIPT_DIR"

g++ -o ../build/hotreload ../src/hotreload/hotreload.cpp -ldl
