#include <string>
#include <vector>
#include <utility>
#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include "approxtimer.h"
#include "colorizedoutput.h"
#include "config.h"
#include "epollhandler.h"
#include "forkpty.h"
#include "helpers.h"

void printUsage()
{
    std::cout << "Usage: cppchroma <program> [args...]" << std::endl;
}

int forwardStdinToMaster(int masterFd, char* buffer, size_t bufferSize)
{
    ssize_t count = read(STDIN_FILENO, buffer, bufferSize);
    if (count > 0)
        return Helpers::writeAllToFd(masterFd, buffer, count);
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
        return count;
    else if (count == 0) // EOF on masterFd
        return -1;
    else if (errno == EAGAIN || errno == EWOULDBLOCK)
        return 0; // Maybe later
    return -1;
}

int runImpl(int argc, char* argv[])
{
    if (argc < 2)
    {
        printUsage();
        return EXIT_SUCCESS;
    }

    Config config;
    config.load();

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

    char stdinBuffer[8192];
    bool stdinJustClosed = false;
    bool masterFdOpen = true;

    Helpers::setNonBlocking(STDOUT_FILENO);

    EpollHandler<2> epollHandler;
    epollHandler.addFd(masterFd, EPOLLIN | EPOLLRDHUP);
    epollHandler.addFd(STDIN_FILENO, EPOLLIN | EPOLLRDHUP);

    ColorizedOutput colorizedOutput(config);

    auto processEvents = [&](int fd, uint32_t events)
    {
        if (fd == masterFd)
        {
            if (events & EPOLLIN)
            {
                auto [bufferPtr, bufferSize] = colorizedOutput.getBuffer();
                ssize_t count = readMasterFd(masterFd, bufferPtr, bufferSize);
                if (count > 0)
                    colorizedOutput.onDataAdded(count);
                else if (count < 0)
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
                    auto [bufferPtr, bufferSize] = colorizedOutput.getBuffer();
                    ssize_t count = readMasterFd(masterFd, bufferPtr, bufferSize);
                    if (count > 0)
                        colorizedOutput.onDataAdded(count);
                    else if (count < 0)
                        break;
                }

                masterFdOpen = false;
                return false;
            }
        }
        else if (fd == STDIN_FILENO && (events & EPOLLIN))
        {
            ssize_t count = forwardStdinToMaster(masterFd, stdinBuffer, sizeof(stdinBuffer));
            if (count < 0) // Done reading stdin
                stdinJustClosed = true;
        }

        return true;
    };

    while (masterFdOpen)
    {
        epollHandler.poll(processEvents, -1);

        if (stdinJustClosed)
        {
            epollHandler.removeFd(STDIN_FILENO);
            stdinJustClosed = false;
        }
    }

    colorizedOutput.flush();
    cwdTimer.stop();

    return forkPtyHelper.waitPid();
}

int run(int argc, char* argv[])
{
    // Silence warnings when loading regex configuration
    absl::InitializeLog();
    absl::SetStderrThreshold(absl::LogSeverity::kFatal);

    try
    {
        return runImpl(argc, argv);
    }
    catch (const std::exception &e)
    {
        LOG_ERROR(e.what());
        return EXIT_FAILURE;
    }
    catch (...)
    {
        LOG_ERROR("cppchroma failed due to unknown exception");
    }

    return EXIT_FAILURE;
}
