#ifndef PROACTOR_HPP
#define PROACTOR_HPP

#pragma once
#include <functional>
#include <unordered_map>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

typedef void (*AsyncCallbackFunction)(int);
typedef void* (*proactorFunc)(int);

class Proactor {
public:
    Proactor();
    ~Proactor();

    void addFd(int fd, AsyncCallbackFunction func);
    void removeFd(int fd);
    void run();
    void stop();

private:
    void handleEvent(int fd);

    int epollFileDescriptor;
    bool active;
    std::mutex fdHandlersMutex;
    std::condition_variable cv;
};

// starts new proactor and returns proactor thread id.
pthread_t startProactor (int sockfd, proactorFunc threadFunc);
// stops proactor by threadid
int stopProactor(pthread_t tid);

#endif // PROACTOR_HPP