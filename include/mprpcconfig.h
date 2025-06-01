#ifndef MPRPCCONFIG_H
#define MPRPCCONFIG_H

#include <unordered_map>
#include <string>

// rpcserverip port zookeeperip port
// 框架读取配置文件类
class MprpcConfig
{
public:
    // 负责解析加载配置文件
    void LoadConfigFile(const char *configFile);
    //  查询配置项信息
    std::string Load(const std::string& key);

private:
    std::unordered_map<std::string, std::string> configMap_;
    // 去点字符串前后的空格
    void trim(std::string &strbuf);
};

#endif // MPRPCCONFIG_H