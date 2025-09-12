#pragma once

#include <utility>
#include <unistd.h>
#include <pty.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include "helpers.h"

class ForkPtyHelper
{
public:
    ForkPtyHelper() {}
    ~ForkPtyHelper()
    {
        waitPid();
    }

    int waitPid()
    {
        closeMasterFd();

        _childReturnCode = EXIT_FAILURE;

        if (_pid != -1)
        {
            int childExitStatus;
            if (waitpid(_pid, &childExitStatus, 0) < 0)
            {
                LOG_PERROR("waitpid");
                _childReturnCode = EXIT_FAILURE;
            }
            else if (WIFEXITED(childExitStatus))
                _childReturnCode = WEXITSTATUS(childExitStatus);

            _pid = -1;
        }

        return _childReturnCode;
    }

    std::pair<pid_t, int> forkAndExec(const char *program, char* const argv[])
    {
        bool originalTermiosIsValid = true;

        // Save the current terminal attributes
        if (tcgetattr(STDIN_FILENO, &_originalTermios) == -1)
            originalTermiosIsValid = false;

        // Set the terminal to raw mode
        if (originalTermiosIsValid)
        {
            // Ensure the terminal attributes are restored at exit
            atexit([]()
            {
                tcsetattr(STDIN_FILENO, TCSANOW, &_originalTermios);
            });

            struct termios rawTermios = _originalTermios;
            cfmakeraw(&rawTermios);
            tcsetattr(STDIN_FILENO, TCSANOW, &rawTermios);
        }

        _masterFd = -1;
        _pid = forkpty(&_masterFd, nullptr, nullptr, nullptr);

        if (_pid == -1)
        {
            LOG_PERROR("forkpty");
            return { -1, -1 };
        }

        // Child process
        if (_pid == 0)
        {
            // Restore original terminal attributes in the child
            if (originalTermiosIsValid)
                tcsetattr(STDIN_FILENO, TCSANOW, &_originalTermios);
            execvp(program, argv);
            LOG_PERROR("execvp");
            _exit(1);
        }

        // Parent process
        if (isatty(STDIN_FILENO))
        {
            // Update the slave's window size when the terminal window is resized
            auto windowResizeHandler = [](int)
            {
                if (_masterFd == -1)
                    return;
                struct winsize ws;
                if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) < 0)
                    return;
                ioctl(_masterFd, TIOCSWINSZ, &ws);
            };

            // Set up the signal handler for window resize
            struct sigaction sa;
            sa.sa_handler = windowResizeHandler;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGWINCH, &sa, nullptr);

            // Trigger the handler initially to set the window size
            windowResizeHandler(SIGWINCH);
        }

        return { _pid, _masterFd };
    }

    int getMasterFd() const
    {
        return _masterFd;
    }

    void closeMasterFd()
    {
        if (_masterFd != -1)
        {
            close(_masterFd);
            _masterFd = -1;
        }
    }

    int getChildReturnCode() const
    {
        return _childReturnCode;
    }

    pid_t getChildPid() const
    {
        return _pid;
    }

private:
    static inline struct termios _originalTermios {};
    static inline int _masterFd = -1;

    int _childReturnCode = -1;
    pid_t _pid = -1;
};
