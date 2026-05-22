#pragma once
#include <unordered_map>
#include <string>

class MprpcConfig
{
public:
    void LoadConfigFile(const char* const configFile);
    std::string Load(const std::string& key);
private:
    std::unordered_map<std::string, std::string> m_configMap;
};