
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

class some_busy_job
{
private:
    some_busy_job();
    ~some_busy_job();
    std::thread t1;
    some_busy_job(const some_busy_job&) = delete;
    some_busy_job& operator=(const some_busy_job&) = delete;

public:
    static some_busy_job& getInstance() {
        static some_busy_job instance;
        return instance;
    }

    void startBusyJob();
    std::string add_something(const std::string& input);
    std::atomic<int> busy_job_value{0};
    std::atomic<bool> running{true};
};


