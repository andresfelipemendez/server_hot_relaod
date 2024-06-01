#ifndef SERVER_H
#define SERVER_H

#include "../hotreload/arena_allocator.h"


extern "C" {

    struct ServerState {
        int server_socket;
        // Add any other persistent state here
    };
    // Function to initialize the server
    int init(ArenaAllocator* allocator, ServerState* state);

    // Function to update the server
    int update(ArenaAllocator* allocator, ServerState* state);

    // Function to shutdown the server
    void server_shutdown(ArenaAllocator* allocator, ServerState* state);
}

#endif // SERVER_H
