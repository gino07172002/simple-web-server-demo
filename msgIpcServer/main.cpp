
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <unistd.h>
#include <thread>

#define QUEUE_NAME "/my_message_queue"
#define REPLY_QUEUE_NAME "/my_reply_queue"
#define MAX_SIZE 1024
#define MSG_STOP "exit"

int main() {
    mqd_t mq, reply_mq;
    struct mq_attr attr;
    char buffer[MAX_SIZE + 1];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;


    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        std::cerr << "Server: mq_open failed, errno: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }


    int retries = 5;  // retry times
    int delay_seconds = 1;  // retry interval delay

    // retry some times to connect the message queue
    while ((reply_mq = mq_open(REPLY_QUEUE_NAME, O_WRONLY)) == (mqd_t)-1 && retries > 0) {
        std::cerr << "Client: mq_open failed, errno: " << strerror(errno) << std::endl;
        std::cerr << "Client: Retrying in " << delay_seconds << " seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
        retries--;
    }

    if (reply_mq == (mqd_t)-1) {
        std::cerr << "Client: Failed to open message queue after retries. Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }



    std::cout << "Server: Waiting for messages..." << std::endl;

    while (true) {
        ssize_t bytes_read;
        memset(buffer, 0, sizeof(buffer));

        // receive message from client
        bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL);
        if (bytes_read >= 0) {
            buffer[bytes_read] = '\0';
            std::cout << "Server: Received message: " << buffer << std::endl;

            // response and print the message
            std::string reply_message = "Server received: " + std::string(buffer);
            if (mq_send(reply_mq, reply_message.c_str(), reply_message.size() + 1, 0) == -1) {
                std::cerr << "Server: mq_send to reply queue failed, errno: " << strerror(errno) << std::endl;
            }
        } else {
            std::cerr << "Server: mq_receive failed, errno: " << strerror(errno) << std::endl;
        }

        if (std::string(buffer) == MSG_STOP) {
            break;
        }
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    mq_close(reply_mq);
    std::cout << "Server: Exiting..." << std::endl;

    return 0;
}
