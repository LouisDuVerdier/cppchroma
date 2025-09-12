#include <string>
#include <vector>
#include <utility>
#include "approxtimer.h"
#include "epollhandler.h"
#include "forkpty.h"
#include "helpers.h"

void printUsage()
{
    std::cout << "Usage: cppchroma <program> [args...]" << std::endl;
}

ssize_t writeAllToFd(int fd, const char *buf, size_t bufferSize)
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

int forwardStdinToMaster(int masterFd, char* buffer, size_t bufferSize)
{
    ssize_t count = read(STDIN_FILENO, buffer, bufferSize);
    if (count > 0)
        return writeAllToFd(masterFd, buffer, count);
    else if (count == 0) // EOF on stdin
        return -1;
    else if (errno == EAGAIN || errno == EWOULDBLOCK)
        return 0; // Maybe later
    return -1;
}

int readMasterFd(int masterFd, char* buffer, size_t bufferSize)
{
    ssize_t count = read(masterFd, buffer, bufferSize);
    if (count > 0)
    {
        writeAllToFd(STDOUT_FILENO, buffer, count);
        return count;
    }
    else if (count == 0) // EOF on masterFd
        return -1;
    else if (errno == EAGAIN || errno == EWOULDBLOCK)
        return 0; // Maybe later
    return -1;
}

int run(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return EXIT_SUCCESS;
    }

    ForkPtyHelper forkPtyHelper;
    auto [pid, masterFd] = forkPtyHelper.forkAndExec(argv[1], &argv[1]);

    ApproxTimer cwdTimer;
    std::string procPath = "/proc/" + std::to_string(pid) + "/cwd";

    // Periodically set the current working directory from the one of the child process
    // Required by certain GUI terms that track CWDs to display them on tabs
    cwdTimer.start(std::chrono::milliseconds(500), [procPath]()
    {
        char cwd[2048];
        ssize_t len = readlink(procPath.c_str(), cwd, sizeof(cwd) - 1);
        if (len != -1)
        {
            cwd[len] = '\0';
            UNUSED(chdir(cwd));
        }
    });

    char buffer[8192];
    bool stdinOpen = true;
    bool masterFdOpen = true;

    Helpers::setNonBlocking(STDOUT_FILENO);

    EpollHandler<2> epollHandler;
    epollHandler.addFd(masterFd, EPOLLIN | EPOLLRDHUP);
    epollHandler.addFd(STDIN_FILENO, EPOLLIN | EPOLLRDHUP);

    auto processEvents = [&](int fd, uint32_t events)
    {
        if (fd == masterFd)
        {
            if (events & EPOLLIN)
            {
                ssize_t count = readMasterFd(masterFd, buffer, sizeof(buffer));
                if (count < 0)
                {
                    masterFdOpen = false;
                    return false;
                }
            }

            if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                // Drain any remaining data
                while (true)
                {
                    ssize_t count = readMasterFd(masterFd, buffer, sizeof(buffer));
                    if (count <= 0 || errno == EAGAIN || errno == EWOULDBLOCK)
                        break;
                }

                masterFdOpen = false;
                return false;
            }
        }
        else if (fd == STDIN_FILENO && (events & EPOLLIN))
        {
            ssize_t count = forwardStdinToMaster(masterFd, buffer, sizeof(buffer));
            if (count < 0) // Done reading stdin
                stdinOpen = false;
        }

        return true;
    };

    while (masterFdOpen)
    {
        epollHandler.poll(processEvents, -1);

        if (!stdinOpen)
        {
            epollHandler.removeFd(STDIN_FILENO);
            stdinOpen = false;
        }
    }

    cwdTimer.stop();

    return forkPtyHelper.waitPid();
}
