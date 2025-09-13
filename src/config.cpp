#include <array>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "config.h"
#include "defaultconfig.h"

Config::Config()
{
    std::array<std::string, 3> configPaths =
    {
        getenv("CPPCHROMA_CONFIG_PATH") ? std::string(getenv("CPPCHROMA_CONFIG_PATH")) : "",
        std::string(getenv("HOME") ? getenv("HOME") : ".") + "/.cppchroma_config.yaml",
        "./cppchroma_config.yaml"
    };

    for (const auto &path : configPaths)
    {
        if (!path.empty() && std::filesystem::exists(path))
        {
            _configPath = path;
            break;
        }
    }
}

void Config::load()
{
    if (_configPath.empty())
    {
        _configPath = std::string(getenv("HOME") ? getenv("HOME") : ".") + "/.cppchroma_config.yaml";
        writeDefaultConfig();
    }

    try
    {
        loadFromFile(_configPath);
    }
    catch (const std::exception &e)
    {
        throw std::runtime_error(std::string("Failed to load configuration from '") + _configPath + "': " + e.what());
    }
}

void Config::loadFromFile(const std::string &path)
{
    YAML::Node config = YAML::LoadFile(path);

    if (!config["rules"] || !config["rules"].IsSequence())
        throw std::runtime_error("Invalid configuration: 'rules' section missing or not a sequence");

    _rules.clear();

    int ruleIndex = 0;

    for (const auto &ruleNode : config["rules"])
    {
        ++ruleIndex;

        if (!ruleNode["description"] || !ruleNode["regex"] || !ruleNode["color"])
            throw std::runtime_error(std::string("Invalid rule entry: missing 'description', 'regex', or 'color' in rule ") + std::to_string(ruleIndex));

        std::string description = ruleNode["description"].as<std::string>();
        std::string regexStr = ruleNode["regex"].as<std::string>();
        std::string colorStr = ruleNode["color"].as<std::string>();

        _rules.emplace_back(std::make_unique<Rule>(regexStr, toAnsiColor(colorStr)));

        const RulePtr &rule = _rules.back();
        if (!rule->regex.ok())
            throw std::runtime_error(std::string("Invalid regex in rule ") + std::to_string(ruleIndex) + " (" + description + "): " + rule->regex.error());
    }
}

void Config::writeDefaultConfig()
{
    std::ofstream configFile(_configPath);
    if (configFile)
    {
        configFile << DEFAULT_CONFIG_STR;
        configFile.close();
    }
}

std::string Config::toAnsiColor(const std::string &colorStr) const
{
    if (colorStr.size() != 8 || (colorStr[0] != 'f' && colorStr[0] != 'b') || colorStr[1] != '#')
        throw std::runtime_error("Invalid color format: " + colorStr + ". Expected format: f#RRGGBB or b#RRGGBB");

    auto hex_to_int = [](const std::string &s)
    {
        int value = std::strtol(s.c_str(), nullptr, 16);
        if (value < 0 || value > 255)
            throw std::runtime_error("Invalid hex color component: " + s);
        return value;
    };

    int r = hex_to_int(colorStr.substr(2, 2));
    int g = hex_to_int(colorStr.substr(4, 2));
    int b = hex_to_int(colorStr.substr(6, 2));

    return std::string(colorStr[0] == 'f' ? "\033[38;2;" : "\033[48;2;") + std::to_string(r) + ";" + std::to_string(g) + ";" + std::to_string(b) + "m";
}
