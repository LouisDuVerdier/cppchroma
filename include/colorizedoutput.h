#pragma once

#include <array>
#include <limits>
#include "config.h"

class ColorizedOutput
{
    static constexpr size_t MAX_BUFFER_SIZE = 65535;
    static constexpr size_t MAX_MATCHES = 4096;

    struct Match
    {
        size_t start;
        size_t len;
        size_t ruleIndex;
    };

public:
    // 5msec before flushing the tail of the buffer, to give time for new data to come in
    static constexpr size_t BUFFERING_DELAY_MSEC = 5;

    ColorizedOutput(const Config &config);

    std::pair<char *, size_t> getBuffer() { return {_buffer.data() + _bufferLen, _buffer.size() - _bufferLen}; }
    bool hasPendingData() const { return _bufferLen > 0; }
    void onDataAdded(size_t dataSize);

    void flushIfTimedOut();
    void flush();

private:
    void process(size_t dataSize);
    void onDataProcessed(size_t dataSize);

private:
    const std::vector<RulePtr> &_rules;
    std::vector<Match> _matches;

    std::array<char, MAX_BUFFER_SIZE> _buffer;
    size_t _bufferLen = 0;
    int64_t _nextFlushTime = std::numeric_limits<int64_t>::max();
};
