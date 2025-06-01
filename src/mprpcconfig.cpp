#include "mprpcconfig.h"

#include <iostream>
#include <string>

void MprpcConfig::LoadConfigFile(const char *configFile)
{
    FILE *pf = fopen(configFile, "r");
    if (pf == nullptr)
    {
        std::cout << configFile << " is not exist" << std::endl;
        exit(EXIT_FAILURE);
    }
    while (!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        // 去字符串前的空格
        std::string strbuf(buf);
        trim(strbuf);

        // 判断#的注释
        if (strbuf[0] == '#' || strbuf.empty())
        {
            continue;
        }

        // 解析配置项
        int idx = strbuf.find('=');
        if (idx == -1)
        {
            // 配置项不合格
            continue;
        }

        std::string key;
        std::string value;
        key = strbuf.substr(0, idx);
        trim(key);
        int endIdx = strbuf.find('\n', idx);
        value = strbuf.substr(idx + 1, endIdx - idx - 1);
        trim(value);

        configMap_.emplace(key, value);
    }
}

std::string MprpcConfig::Load(const std::string &key)
{
    auto it = configMap_.find(key);
    if (it == configMap_.end())
    {
        return "";
    }
    return it->second;
}

void MprpcConfig::trim(std::string &strbuf)
{
    int idx = strbuf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符前有空格
        strbuf = strbuf.substr(idx, strbuf.size() - idx);
    }
    // 去字符串后的空格
    idx = strbuf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符后有空格
        strbuf = strbuf.substr(0, idx + 1);
    }
}
