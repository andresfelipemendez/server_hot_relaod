#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "server.h"

static ArenaAllocator* global_allocator = nullptr;
static ServerState* global_state = nullptr;

int init(ArenaAllocator* allocator, ServerState* state) {
    global_allocator = allocator;
    global_state = state;

    std::cout << "Server initialized" << std::endl;

    // Create socket
    state->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (state->server_socket < 0) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // Bind socket to a port
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(state->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        return -1;
    }

    // Start listening for connections
    if (listen(state->server_socket, 10) < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return -1;
    }

    return 0;
}

int update(ArenaAllocator* allocator, ServerState* state) {
    std::cout << "Server updating" << std::endl;

    // Accept new connections
    sockaddr_in client_addr = {};
    socklen_t client_len = sizeof(client_addr);
    int client_socket = accept(state->server_socket, (struct sockaddr*)&client_addr, &client_len);

    if (client_socket < 0) {
        std::cerr << "Failed to accept connection" << std::endl;
        return -1;
    }

    // Handle client request (simple HTTP response)
    const char* http_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "Hello, HI!";

    write(client_socket, http_response, strlen(http_response));
    close(client_socket);

    return 0;
}

void server_shutdown(ArenaAllocator* allocator, ServerState* state) {
    std::cout << "Server shutting down" << std::endl;
    close(state->server_socket);
}
