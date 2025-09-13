#pragma once

#include <array>
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
    ColorizedOutput(const Config &config);

    std::pair<char *, size_t> getBuffer() { return {_buffer.data() + _bufferLen, _buffer.size() - _bufferLen}; }
    void onDataAdded(size_t dataSize);
    void flush();

private:
    void process(size_t dataSize);
    void onDataProcessed(size_t dataSize);

private:
    const std::vector<RulePtr> &_rules;
    std::vector<Match> _matches;

    std::array<char, MAX_BUFFER_SIZE> _buffer;
    size_t _bufferLen = 0;
};
