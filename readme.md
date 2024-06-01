# Hot Reloadable Web Server with Arena Allocator

This project is a simple web server with hot-reload capabilities, implemented in C++. The server can dynamically reload its shared library (`.so` file) without restarting, preserving its state using an arena allocator.

## Features

- **Hot Reloading**: Automatically reloads the shared library when changes are detected.
- **Arena Allocator**: Efficient memory management using a custom arena allocator.
- **Persistent State**: Maintains server state across library reloads.
- **Simple HTTP Server**: Responds with "Hello, World!" to incoming HTTP requests.

## Directory Structure
```
.
├── build
├── buildscripts
│ ├── build_hotreload.sh
│ └── build_lib.sh
├── hotreload
│ ├── arena_allocator.h
│ ├── hotreload.cpp
├── lib
├── run.sh
└── src
├── server.cpp
└── server.h
```

## Getting Started

### Prerequisites

- **Git**: To clone the repository.
- **C++ Compiler**: To build the source code (e.g., `g++`).

### Clone the Repository

```sh
git clone https://github.com/andresfelipemendez/server_hot_reload.git
cd hot-reloadable-web-server
```

## Building the Project

 1)  Build the Shared Library (Server)
 ```sh
 ./buildscripts/build_lib.sh
 ```
 2) Build the Hot Reload Watcher
```sh
 ./buildscripts/build_hotreload.sh
 ```

 ## Running the Server
 ```sh
 ./run.sh
 ```

## Usage
The server will start and listen on port 8080.
You can access the server by navigating to http://localhost:8080 in your web browser.
The server responds with "Hello, World!".
## Hot Reloading
The hot reload watcher will monitor changes to the shared library (enginelib.so).
When a change is detected, the library is reloaded without restarting the server, preserving the state using the arena allocator.

## Example Output
```bash
Server initialized
Server updating
Server updating
DLL updated. Reloading...
Server updating
```
