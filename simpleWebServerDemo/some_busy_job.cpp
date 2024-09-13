#include "some_busy_job.h"

some_busy_job::some_busy_job() {
    std::cout << "init busy job~" << std::endl;
    t1 = std::thread(std::bind(&some_busy_job::startBusyJob, this));
};


some_busy_job::~some_busy_job() {
    std::cout << "end busy job~" << std::endl;
    running=false;
    if(t1.joinable())
        t1.join();

};
void some_busy_job::startBusyJob()
{
    while(this->running)  //doing some busy job
    {
        this->busy_job_value++;
        sleep(5);
    }
}

std::string some_busy_job::add_something(const std::string& input)
{

    busy_job_value+=input.size();
    std::string result= std::to_string(busy_job_value.load());

    return   result;
}
