#pragma once

#include <string>
#include <vector>
#include <memory>
#include <re2/re2.h>

struct Rule
{
    re2::RE2 regex;
    std::string colorPrefix; // Color converted to ANSI escape sequence
    static constexpr std::string_view colorSuffix{"\033[0m"};

    Rule(const std::string &pattern, const std::string &ansiColor)
        : regex(pattern), colorPrefix(ansiColor)
    {}
};

using RulePtr = std::unique_ptr<Rule>;

class Config
{
public:
    Config();

    void load();

    static void writeDefaultConfig(const std::string &path);

    const std::vector<RulePtr> &rules() const { return _rules; }
    const std::string &configPath() const { return _configPath; }

private:
    void loadFromFile(const std::string &path);
    std::string toAnsiColor(const std::string &colorStr) const;

private:
    std::vector<RulePtr> _rules;
    std::string _configPath;
};
