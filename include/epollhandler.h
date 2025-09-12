#pragma once

#include <sys/epoll.h>
#include <unistd.h>
#include <stdexcept>

template<size_t MAX_EVENTS>
class EpollHandler
{
public:
    EpollHandler()
    {
        memset(_events, 0, sizeof(_events));

        _epollFd = epoll_create1(0);
        if (_epollFd == -1)
            throw std::runtime_error("Failed to create epoll file descriptor");
    }

    EpollHandler(const EpollHandler&) = delete;
    EpollHandler& operator=(const EpollHandler&) = delete;

    ~EpollHandler()
    {
        close(_epollFd);
        _epollFd = -1;
    }

    void addFd(int fd, uint32_t events)
    {
        if (_fdCount >= MAX_EVENTS)
            throw std::runtime_error("Maximum number of file descriptors reached");

        auto &event = _events[_fdCount++];
        event.data.fd = fd;
        event.events = events;

        if (epoll_ctl(_epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
            throw std::runtime_error("Failed to add file descriptor to epoll");
    }

    void removeFd(int fd)
    {
        for (size_t i = 0; i < _fdCount; ++i)
        {
            if (_events[i].data.fd == fd)
            {
                if (epoll_ctl(_epollFd, EPOLL_CTL_DEL, fd, nullptr) == -1)
                    throw std::runtime_error("Failed to remove file descriptor from epoll");

                for (size_t j = i; j < _fdCount - 1; ++j)
                    _events[j] = _events[j + 1];

                --_fdCount;
                return;
            }
        }
    }

    template<typename Callable>
    void poll(Callable &callable, int timeout = -1)
    {
        int nfds = epoll_wait(_epollFd, _events, _fdCount, timeout);
        if (nfds == -1 && errno != EINTR)
            throw std::runtime_error("epoll_wait failed");

        for (int i = 0; i < nfds; ++i)
        {
            int fd = _events[i].data.fd;
            uint32_t evs = _events[i].events;

            if (!callable(fd, evs))
                break;
        }
    }

private:
    int _epollFd = -1;
    size_t _fdCount = 0;
    struct epoll_event _events[MAX_EVENTS];
};
