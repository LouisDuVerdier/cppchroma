#pragma once

#include <iostream>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <stdexcept>

#define USE_LOG_DEBUG 1

#if USE_LOG_DEBUG
# define LOG_DEBUG(log) \
    do { \
        std::cout << __FILE__ << ":" << __LINE__ << ": " << log << std::endl; \
    } while (0)
#else
# define LOG_DEBUG(log) \
    do {} while (0)
#endif

#define LOG_ERROR(log) \
    do { \
        std::cerr << __FILE__ << ":" << __LINE__ << ": " << log << std::endl; \
    } while (0)

#define LOG_PERROR(log) \
    do { \
        std::cerr << __FILE__ << ":" << __LINE__ << ": " << log << ": " << std::strerror(errno) << std::endl; \
    } while (0)

namespace Helpers
{
    template<typename T>
    inline void unused(const T &&) {}

    inline void setNonBlocking(int fd)
    {
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1)
            throw std::runtime_error("fcntl F_GETFL failed");
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            throw std::runtime_error("fcntl F_SETFL O_NONBLOCK failed");
    }

    inline ssize_t writeAllToFd(int fd, const char *buf, size_t bufferSize)
    {
        const char *p = buf;
        size_t bytesLeft = bufferSize;

        while (bytesLeft)
        {
            ssize_t written = write(fd, p, bytesLeft);
            if (written > 0)
            {
                bytesLeft -= written;
                p += written;
                continue;
            }
            if (written == -1 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK))
                continue; // Don't want to miss some data to write
            return -1;
        }

        return (ssize_t)bufferSize;
    }
}

#define UNUSED(x) Helpers::unused(x)
