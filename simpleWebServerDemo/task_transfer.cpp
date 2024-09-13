#include "task_transfer.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <thread>
#include <chrono>
#include <arpa/inet.h>
#include <fcntl.h>
#define TEXT_SIZE 2048
#define QUEUE_NAME "/my_message_queue"
#define REPLY_QUEUE_NAME "/my_reply_queue"
#define MAX_SIZE 1024
#define MSG_STOP "exit"


void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
    }
}
task_transfer::task_transfer()
{

}
std::string process_transfer::sendTo()
{
    std::string res= myIpc.sendMessage();
    return  res;
}

process_transfer::~process_transfer()
{


}

std::string socket_transfer::runSocket()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char request[1024]= {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return "";
    }

    setNonBlocking(sock);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5321);


    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {  //this can be set to other ip if the task handler is in other computer
        std::cout << "Invalid address/ Address not supported" << std::endl;
        return "";
    }


    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));


    std::string response;

    while (true) {

        fd_set write_fds, read_fds;
        struct timeval timeout;

        // Clear and set the file descriptors for writing
        FD_ZERO(&write_fds);
        FD_SET(sock, &write_fds);

        timeout.tv_sec = 5;  // Timeout of 5 seconds for writing
        timeout.tv_usec = 0;

        // Check if the socket is ready for writing (send)
        if (select(sock + 1, NULL, &write_fds, NULL, &timeout) > 0) {

            send(sock, request, sizeof(request)/sizeof(request[0]), 0);
        } else {
            std::cout << "Send timeout, skipping..." << std::endl;
            continue;
        }

        // Clear and set the file descriptors for reading
        FD_ZERO(&read_fds);
        FD_SET(sock, &read_fds);

        timeout.tv_sec = 5;  // Timeout of 5 seconds for reading
        timeout.tv_usec = 0;

        // Check if the socket is ready for reading (receive)
        if (select(sock + 1, &read_fds, NULL, NULL, &timeout) > 0) {
            int valread = read(sock, buffer, 1024);
            if (valread > 0) {
                std::cout<<" get response buffer: "<<buffer<<std::endl;

                response=std::string(buffer);
                if(!response.empty())
                    break;
            } else if (valread == 0) {
                std::cout << "Server disconnected, reconnecting..." << std::endl;
                close(sock);
                if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                    std::cout << "Socket creation error" << std::endl;
                    return "";
                }
                setNonBlocking(sock);
                connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            }
        } else {
            std::cout << "Receive timeout or error." << std::endl;
        }

        std::cout<<std::endl;
        std::cout<<std::endl;

        sleep(1); // you should always has sleep in select loop ( maybe in milliseconds in real application situation ) or it may overheat and shutdown your computer especially in raspberry pi

    }

    close(sock);
    return response;
}


