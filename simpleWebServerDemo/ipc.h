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

#define QUEUE_NAME "/my_message_queue"
#define REPLY_QUEUE_NAME "/my_reply_queue"
#define MAX_SIZE 1024


class ipc  // a standard singleton structure
{
private:
    ipc() {
        std::cout << "init ipc~" << std::endl;
    };

    ipc(const ipc&) = delete;  // to prevent instance be delete or copy by user
    ipc& operator=(const ipc&) = delete;

public:
    static ipc& getInstance() {
        static ipc instance;
        return instance;
    }
    mqd_t mq, reply_mq;


    void startMqConnect();
    void closeMqConnect();
    std::string sendMessage();
};

