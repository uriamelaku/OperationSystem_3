#include "proactor.hpp"
#include <sys/epoll.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <unordered_map>
#include <mutex>

using namespace std;

std::unordered_map<int, AsyncCallbackFunction> fdHandlers;
std::mutex fdHandlersMutex;

Proactor::Proactor() : active(false) {
    epollFileDescriptor = epoll_create1(0);
    if (epollFileDescriptor == -1) {
        std::cerr << "Failed to create epoll file descriptor" << std::endl;
        exit(EXIT_FAILURE);
    }
}

Proactor::~Proactor() {
    stop();
    close(epollFileDescriptor);
}

void Proactor::addFd(int fd, AsyncCallbackFunction func) {
    std::lock_guard<std::mutex> lock(fdHandlersMutex);

    fdHandlers[fd] = func;
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;

    if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_ADD, fd, &ev) == -1) {
        std::cerr << "Failed to add file descriptor to epoll" << std::endl;
        fdHandlers.erase(fd);
    }
}

void Proactor::removeFd(int fd) {
    std::lock_guard<std::mutex> lock(fdHandlersMutex);

    if (fdHandlers.erase(fd) > 0) {
        if (epoll_ctl(epollFileDescriptor, EPOLL_CTL_DEL, fd, nullptr) == -1) {
            std::cerr << "Failed to remove file descriptor from epoll" << std::endl;
        }
    }
}

void Proactor::run() {
    active = true;
    const int maxEvents = 100;
    struct epoll_event events[maxEvents];

    while (active) {
        int n_events = epoll_wait(epollFileDescriptor, events, maxEvents, -1);
        if (n_events == -1) {
            if (errno == EINTR) continue;
            std::cerr << "epoll_wait failed" << std::endl;
            break;
        }

        for (int i = 0; i < n_events; ++i) {
            int fd = events[i].data.fd;
            std::thread(&Proactor::handleEvent, this, fd).detach();
        }
    }
}

void Proactor::stop() {
    active = false;
}

// Helper function to handle events
void Proactor::handleEvent(int fd) {
    std::lock_guard<std::mutex> lock(fdHandlersMutex);

    auto it = fdHandlers.find(fd);
    if (it != fdHandlers.end()) {
        it->second(fd);
    }
}

proactorFunc threadFunc = nullptr;  // משתנה גלובלי שמצביע על פונקציית התהליכון

// פונקציה שתעטוף את הקריאה ל-threadFunc ותמיר את ה-arg ממצביע ל-int
void* threadFuncWrapper(void* arg) {
    int sockfd = *static_cast<int*>(arg);  // המרת arg חזרה לערך int
    delete static_cast<int*>(arg);         // שחרור זיכרון דינמי

    if (threadFunc != nullptr) {           // בדיקה אם הפונקציה הוגדרה
        return threadFunc(sockfd);         // קריאה לפונקציה שהוגדרה ב-threadFunc
    }

    return nullptr;
}

// פונקציה שמתחילה את ה-Proactor בתהליכון חדש ומחזירה את מזהה ה-thread
pthread_t startProactor(int sockfd, proactorFunc func) {
    pthread_t tid;
    int* sockfdPtr = new int(sockfd);       // הקצאת זיכרון דינמית ל-sockfd
    threadFunc = func;                      // עדכון המשתנה הגלובלי בפונקציה שהתקבלה כפרמטר

    // יצירת התהליכון החדש עם פונקציית ה-wrapper והעברת המצביע ל-sockfd
    if (pthread_create(&tid, nullptr, threadFuncWrapper, sockfdPtr) != 0) {
        std::cerr << "Failed to create proactor thread" << std::endl;
        delete sockfdPtr;                   // שחרור הזיכרון אם יצירת התהליכון נכשלה
        return 0;
    }

    return tid;
}

// פונקציה שעוצרת את ה-Proactor לפי ה-thread id
int stopProactor(pthread_t tid) {
    return pthread_cancel(tid);
}
