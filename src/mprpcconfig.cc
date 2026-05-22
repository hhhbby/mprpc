#include "mprpcconfig.h"
#include <iostream>

void trim(std::string& str)
{
    int idx = str.find_first_not_of(' ');
    int endIdx = str.find_last_not_of(' ');
    if (idx == std::string::npos)
    {
        str = "";
        return;
    }
    str = str.substr(idx, endIdx - idx + 1);
}

void MprpcConfig::LoadConfigFile(const char* configFile)
{
    FILE* fp = fopen(configFile, "r");
    if (!fp)
    {
        std::cout << configFile << " is not exist" << std::endl;
        exit(EXIT_FAILURE);
    }


    while (!feof(fp))
    {
        char buf[512] = {0};
        fgets(buf, sizeof(buf), fp);

        std::string line(buf);
        trim(line);
        if (line.empty() || line[0] == '#')
            continue;

        int idx = line.find('=');
        if (idx == std::string::npos)
        {
            continue;
        }
        std::string key = line.substr(0, idx);
        std::string value = line.substr(idx + 1, line.size() - idx - 2);
        trim(key);
        trim(value);
        m_configMap.insert({key, value});
    }

    fclose(fp);
    std::cout << "LoadConfigFile done, conf items : " << m_configMap.size() << std::endl;
}

std::string MprpcConfig::Load(const std::string& key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
        return "";
    return it->second;
}