#pragma once

#include <chrono>
#include <thread>
#include <atomic>

class ApproxTimer
{
public:
    ApproxTimer() = default;
    ApproxTimer(const ApproxTimer&) = delete;
    ApproxTimer& operator=(const ApproxTimer&) = delete;
    ~ApproxTimer()
    {
        stop();
    }

    template<typename Callable>
    void start(std::chrono::milliseconds interval, Callable &&callback)
    {
        stop();

        _stopped = false;
        _thread = new std::thread([this, interval, &callback]()
        {
            auto timeNow = std::chrono::steady_clock::now();
            auto nextTick = timeNow + interval;

            while (!_stopped)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                if (_stopped)
                    break;
                timeNow = std::chrono::steady_clock::now();
                if (timeNow < nextTick)
                    continue;
                callback();
                nextTick = timeNow + interval;
            }
        });
    }

    void stop()
    {
        _stopped = true;

        if (_thread)
        {
            _thread->join();
            delete _thread;
            _thread = nullptr;
        }
    }

private:
    std::atomic_bool _stopped{true};
    std::thread *_thread = nullptr;
};
