#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "thread_pool.h"
#include "task_transfer.h"

const int PORT = 8080;
const int BACKLOG = 10;
const int BUF_SIZE = 1024;

int set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        std::cerr << "Error: Unable to get socket flags\n";
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        std::cerr << "Error: Unable to set non-blocking mode\n";
        return -1;
    }
    return 0;
}

ThreadPool pool(4);

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S");

    return ss.str();
}


std::string removeTrailingSlash(const std::string& uri) {
    if (!uri.empty() && uri.back() == '/') {
        return uri.substr(0, uri.size() - 1);
    }
    return uri;
}

std::string loadHtmlFromFile(const std::string& filePath) {
    std::ifstream file(filePath);

    //try original filename

    if (!file.is_open())
    {
        file.open(removeTrailingSlash(filePath)+".html");
    }
    //try + .html file name

    if (!file.is_open()) {
        return "<html><body><h1>404 not found: no such page</h1></body></html>";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
bool matchUri(const std::string& uri1, const std::string& uri2) {
    return removeTrailingSlash(uri1) == removeTrailingSlash(uri2);
}

void parseRequest(const std::string& request, std::string& method, std::string& uri,std::string& requestBody) {
    std::istringstream stream(request);
    std::string line;

    bool firstLine=true;
    bool isBody=false;
    std::string body="";
    // find the necessary infomation the http request
    while (std::getline(stream, line)) {

        std::istringstream lineStream(line);
        // extract method type ( GET, POST ... )
        if(firstLine)
        {
            lineStream >> method;
            // extract uri after got method
            lineStream >> uri;
            firstLine=false;
        }
        else if(line.at(0)==(char)13)
        {
            std::string temp;
             try {
               isBody=true;
            }  catch (...) {
                std::cerr<<" bad content length"<<std::endl;
            }
        }
        else if(isBody)
        {
            std::string temp;
            lineStream>>temp;
            body+=temp;
        }

    }

    requestBody=body;
}
std::string routerTable(std::string request)
{

    std::string content= "Hello, World! QQ : " +getCurrentTime();
    std::string method;
    std::string uri;
    std::string requestBody;
    parseRequest(request,method,uri,requestBody);

    std::cout<<" request = "<<request<<std::endl;
    std::cout<<" method = "<<method<<" , uri "<< uri<<std::endl;

    if(method=="OPTION")  // javascript's fetch sometimes send a "OPTION" method before real request ( so-called pre-flight request)
    {
        content="";
    }
    else if(method=="GET"&& matchUri("/api/5566/",uri))
    {

        //immediately replay response
        std::cout<<" hi GET /api/5566/  transfer to handler ! "<<std::endl;

        content = " 5566 is good ";
    }
    else if(method=="POST"&& matchUri("/api/jobInThread",uri))
    {

        std::cout<<" hi GET /api/5566/  transfer to handler ! "<<std::endl;


        // mimic getting a running task's current result, ex: sensor value, camera detection result ...
        thread_transfer transfer;
        content= transfer.sendTo();

    }else if(method=="POST"&& matchUri("/api/jobToIPC",uri))
    {

        // mimic if the task need to be done by periphery program, ex: hardware's abstraction layer program
        std::cout<<" hi POST /api/jobToIPC  transfer to other process ! "<<std::endl;

        // content= ipcHandler.sendMessage();
        process_transfer transfer;
        content=transfer.sendTo();

    }

    else if(method=="POST"&& matchUri("/api/jobToSocket",uri))
    {

        //mimic the task need to send to other computer, ex: Distributed computing
        std::cout<<" hi POST /api/jobToSocket  transfer to other IP ! "<<std::endl;

        socket_transfer transfer;
        content= transfer.sendTo();
    }
    else if(uri.find("/api/")==std::string::npos)  // if the request not /api/ , it seems request a web page
    {

        std::string content =  loadHtmlFromFile("./web/"+uri);

        std::string http_response =
                "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: "+std::to_string(content.size())+"\r\n"
            "\r\n"+content;
        std::cout<<" request in router table : "<<std::endl;


        return http_response;
    }
    else  // finally , it seems noting here, return 404
    {
        std::string http_response =
                "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 0\r\n"
            "\r\n";

        return http_response;

    }

    std::string http_response =
            "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: "+std::to_string(content.size())+"\r\n"
        "\r\n"+content;

    return http_response;
}

int main() {

    ipc& ipcHandler = ipc::getInstance();
    ipcHandler.startMqConnect();


    int server_fd, new_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_size;
    char buffer[BUF_SIZE];
    fd_set master_set, read_fds;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Error: Cannot open socket\n";
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    memset(&(server_addr.sin_zero), '\0', 8);

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Error: setsockopt failed\n";
        return -1;
    }


    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error: Cannot bind socket\n";
        return -1;
    }


    if (listen(server_fd, BACKLOG) == -1) {
        std::cerr << "Error: Cannot listen\n";
        return -1;
    }

    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &master_set);
    set_non_blocking(server_fd);

    int fd_max = server_fd;

    std::cout << "Server is running on port " << PORT << "\n";

    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while (true) {
        read_fds = master_set;

        if (select(fd_max + 1, &read_fds, NULL, NULL, &tv) == -1) {
            std::cerr << "Error: Select failed\n";
            return -1;
        }

        for (int i = 0; i <= fd_max; ++i) {
            if (FD_ISSET(i, &read_fds)) {

                // getting new connection ...
                if (i == server_fd) {
                    sin_size = sizeof(client_addr);
                    new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &sin_size);
                    if (new_fd == -1) {
                        std::cerr << "Error: Cannot accept connection\n";
                    } else {
                        set_non_blocking(new_fd);
                        FD_SET(new_fd, &master_set); // add new connection to fd_set
                        if (new_fd > fd_max) {
                            fd_max = new_fd; // update max file descriptor
                        }
                        std::cout << "New connection\n";
                    }
                }
                // read data if connected
                else {
                    int bytes_received = recv(i, buffer, BUF_SIZE - 1, 0);

                    if (bytes_received <= 0&&FD_ISSET(i, &read_fds)) {

                        // disconnect client
                        if (bytes_received == 0) {
                            std::cout << "Connection closed\n";
                        } else {
                            std::cerr << "Error: recv failed\n";
                        }
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        buffer[bytes_received] = '\0';

                        std::string request(buffer,bytes_received);
                        std::cout<<" getting request set : "<<&master_set<<std::endl;

                        pool.enqueueTask([request,i,&master_set]() {
                            std::cout << "running task fd = "<<i<< std::endl;

                            std::string http_response =  routerTable(request);

                            send(i, http_response.c_str(),http_response.size(), 0);
                            std::cout<<" close : "<<i<<" fdset "<<&master_set<<std::endl;

                            close(i);
                            FD_CLR(i, &master_set); //removed finished fd

                            std::cout<<" response request, disconnect fd ... "<<std::endl;

                        });
                        std::cout<<" let's doing loop continue ... "<<std::endl;
                    }
                }
            }
        }
        usleep(100000);  //should always has sleep in non-blocking socket or it may heat down your CPU
    }
    close(server_fd);
    ipcHandler.closeMqConnect();
    return 0;
}
