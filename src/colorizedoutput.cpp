#include <iostream>
#include <cstring>
#include "colorizedoutput.h"
#include "helpers.h"

ColorizedOutput::ColorizedOutput(const Config &config)
    : _rules(config.rules())
{
    _matches.reserve(MAX_MATCHES);
}

void ColorizedOutput::onDataAdded(size_t dataSize)
{
    _bufferLen += dataSize;
    process(_bufferLen); // May want to limit how much we process, to avoid truncated matches
}

void ColorizedOutput::flush()
{
    process(_bufferLen);
}

void ColorizedOutput::process(size_t dataSize)
{
    _matches.clear();

    std::string_view contents{_buffer.data(), dataSize};

    for (size_t ruleIndex = 0; ruleIndex < _rules.size(); ++ruleIndex)
    {
        re2::StringPiece subject(contents);
        re2::StringPiece match;

        while (RE2::FindAndConsume(&subject, _rules[ruleIndex]->regex, &match) && _matches.size() < MAX_MATCHES)
        {
            Match current{static_cast<size_t>(match.data() - contents.data()), match.size(), ruleIndex};

            // Insert in sorted order
            auto it = std::lower_bound(_matches.begin(), _matches.end(), current,
                [](const Match &a, const Match &b) { return std::tie(a.start, a.ruleIndex, a.len) < std::tie(b.start, b.ruleIndex, b.len); });

            _matches.insert(it, current);
        }
    }

    if (_matches.empty())
    {
        Helpers::writeAllToFd(STDOUT_FILENO, _buffer.data(), dataSize);
        onDataProcessed(dataSize);
        return;
    }

    // Advance in the buffer, writing out colorized segments (may contain conflicts)
    size_t currentBufferPos = 0;
    size_t currentMatchIndex = 0;

    while (currentBufferPos < dataSize && currentMatchIndex < _matches.size())
    {
        const Match &match = _matches[currentMatchIndex];

        if (match.start >= currentBufferPos)
        {
            // Write out any non-matching data before this match
            if (match.start > currentBufferPos)
            {
                Helpers::writeAllToFd(STDOUT_FILENO, _buffer.data() + currentBufferPos, match.start - currentBufferPos);
                currentBufferPos = match.start;
            }

            // Write out the matching data with color
            const RulePtr &rule = _rules[match.ruleIndex];
            Helpers::writeAllToFd(STDOUT_FILENO, rule->colorPrefix.data(), rule->colorPrefix.size());
            Helpers::writeAllToFd(STDOUT_FILENO, _buffer.data() + match.start, match.len);
            Helpers::writeAllToFd(STDOUT_FILENO, Rule::colorSuffix.data(), Rule::colorSuffix.size());
            currentBufferPos += match.len;
        }

        ++currentMatchIndex;
    }

    if (currentBufferPos < dataSize)
        Helpers::writeAllToFd(STDOUT_FILENO, _buffer.data() + currentBufferPos, dataSize - currentBufferPos);

    onDataProcessed(dataSize);
}

void ColorizedOutput::onDataProcessed(size_t dataSize)
{
    if (dataSize < _bufferLen)
    {
        size_t remaining = _bufferLen - dataSize;
        std::memmove(_buffer.data(), _buffer.data() + dataSize, remaining);
        _bufferLen = remaining;
    }
    else
        _bufferLen = 0;
}
