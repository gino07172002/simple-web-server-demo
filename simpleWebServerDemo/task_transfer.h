

#include <iostream>
#include <mqueue.h>
#include <chrono>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <condition_variable>
#include <functional>
#include "ipc.h"
#include "some_busy_job.h"



class task_transfer
{
public:
    task_transfer();
    virtual std::string sendTo(){
        std::cout<<"send task to somewhere ... "<<std::endl;
        return "";
    };
};

class thread_transfer:task_transfer
{
public:
    thread_transfer(){};
    std::string sendTo() override
    {
        std::cout<<" send to thread task  , task doing in other thread in web server process... "<<std::endl;
        some_busy_job& job = some_busy_job::getInstance();

        std::string response=" this is job result : "+ job.add_something("hello");
        return  response;
    }
};

class process_transfer:task_transfer
{
public:
    process_transfer(){

    };
    ~process_transfer();
    std::string sendTo() override;

    void haha(){};
private:

    ipc& myIpc = ipc::getInstance();

    void callMeTest(){
        std::cout<<"call me? "<<std::endl;
    }
};





class socket_transfer:task_transfer
{
public:
    socket_transfer(){};
    std::string sendTo() override
    {
        std::cout<<" send to socket transfer  , task send to socket to other ip "<<std::endl;

        std::cout<<" sending ... ( from socket ) ";
        std::cout<<" waiting for socket response "<<std::endl;
        std::string response= runSocket();
        return response;
    }
    std::string runSocket();
};




