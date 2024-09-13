#include "ipc.h"

void ipc::startMqConnect()
{

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    reply_mq = mq_open(REPLY_QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (reply_mq == (mqd_t)-1) {
        std::cerr << "Client: reply_mq_open failed, errno: " << strerror(errno) << std::endl;
        mq_close(mq);
        return ;
    }


    int retries = 5;  // retry times
    int delay_seconds = 1;   // retry delay

    // try to connect message queue
    while ((mq = mq_open(QUEUE_NAME, O_WRONLY)) == (mqd_t)-1 && retries > 0) {
        std::cerr << "Client: mq_open failed, errno: " << strerror(errno) << std::endl;
        std::cerr << "Client: Retrying in " << delay_seconds << " seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(delay_seconds));
        retries--;
    }

    if (mq == (mqd_t)-1) {
        std::cerr << "Client: Failed to open message queue after retries. Exiting..." << std::endl;

    }



    std::cout << "mq connection done ... " << std::endl;

}

void ipc::closeMqConnect()
{
    mq_close(mq);
    mq_close(reply_mq);
    mq_unlink(REPLY_QUEUE_NAME);
    mq_unlink(QUEUE_NAME);

    std::cout << "Client: Exiting..." << std::endl;
}

std::string ipc::sendMessage()
{
    char buffer[MAX_SIZE];
    char reply_buffer[MAX_SIZE + 1];
    std::string result;


    std::string replay="from simple web server ... ";


    strncpy(buffer,replay.c_str(), replay.size());

    buffer[replay.size()]='\0';
    buffer[replay.size()+1]=10;

    // send task request
    if (mq_send(mq, buffer, strlen(buffer) + 1, 0) == -1) {
        std::cerr << "Client: mq_send failed, errno: " << strerror(errno) << std::endl;

    }


    // get the response from message queue server
    ssize_t bytes_read = mq_receive(reply_mq, reply_buffer, MAX_SIZE, NULL);
    if (bytes_read >= 0) {
        reply_buffer[bytes_read] = '\0';
        result=std::string(reply_buffer);
        std::cout << "Client: Received reply from server: " << reply_buffer << std::endl;
    } else {
        std::cerr << "Client: mq_receive failed, errno: " << strerror(errno) << std::endl;
    }




    return  result;
}
