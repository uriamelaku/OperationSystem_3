#ifndef REACTOR_HPP
#define REACTOR_HPP

#pragma once

#include <functional>
#include <unordered_map>
#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

using CallbackFunction = std::function<void(int)>;

class Reactor {
public:
    Reactor();
    ~Reactor();

    void addFd(int fd, CallbackFunction func);
    void removeFd(int fd);
    void run();
    void stop();

private:
    int epollFileDescriptor;
    bool active;
    std::unordered_map<int, CallbackFunction> fdHandlers;
};

#endif // REACTOR_HPP