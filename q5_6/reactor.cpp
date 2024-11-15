#include "reactor.hpp"

Reactor::Reactor() : active(false) {
    epollFileDescriptor = epoll_create1(0);
    if (epollFileDescriptor == -1) {
        std::cerr << "Failed to create epoll file descriptor\n";
        exit(EXIT_FAILURE);
    }
}

Reactor::~Reactor() {
    close(epollFileDescriptor);
}

void Reactor::addFd(int fd, CallbackFunction func) {
    struct epoll_event epEvent;
    epEvent.data.fd = fd;
    epEvent.events = EPOLLIN;

    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, fd, &epEvent) == -1) {
        std::cerr << "Error adding fd to epoll\n";
        exit(EXIT_FAILURE);
    }

    fdHandlers[fd] = func;
}

void Reactor::removeFd(int fd) {
    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_DEL, fd, nullptr) == -1) {
        std::cerr << "Error removing fd from epoll\n";
        exit(EXIT_FAILURE);
    }

    fdHandlers.erase(fd);
}

void Reactor::run() {
    active = true;
    const int maxEvents = 64;
    struct epoll_event eventList[maxEvents];

    while (active) {
        int eventCount = epoll_wait(epollFileDescriptor, eventList, maxEvents, -1);
        if (eventCount == -1) {
            std::cerr << "Error waiting for events\n";
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < eventCount; ++i) {
            int fd = eventList[i].data.fd;
            if (fdHandlers.count(fd) > 0) {
                fdHandlers[fd](fd);
            }
        }
    }
}

void Reactor::stop() {
    active = false;
}
