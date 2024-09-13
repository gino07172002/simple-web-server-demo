#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 5321
#define BUFFER_SIZE 1024

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char* response = "Message received!";

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        std::cerr << "Socket failed" << std::endl;
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Bind failed" << std::endl;
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen failed" << std::endl;
        close(server_fd);
        return -1;
    }

    std::cout << "Server is waiting for connections..." << std::endl;

    // a main loop to wait client connection
    while (true) {
        std::cout << "Waiting for a new client connection..." << std::endl;

        // accept connect of client
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            std::cerr << "Accept failed" << std::endl;
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        // read message from client
        int bytesRead = read(new_socket, buffer, BUFFER_SIZE);
        if (bytesRead > 0) {
            std::cout << "Message from client: " << buffer << std::endl;

            // send message to client
            send(new_socket, response, strlen(response), 0);
            std::cout << "Response sent to client." << std::endl;
        } else {
            std::cerr << "Error reading from client or client disconnected." << std::endl;
        }

        // close connect of client and wait next connection
        close(new_socket);
        memset(buffer, 0, BUFFER_SIZE); // clear buffer
        usleep(10000);
    }

    // close server
    close(server_fd);
    return 0;
}
