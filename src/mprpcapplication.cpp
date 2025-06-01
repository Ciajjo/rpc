#include "mprpcapplication.h"

#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::config_;

void ShowArgsHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}

void MprpcApplication::init(int argc, char **argv)
{
    if (argc < 2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c = 0;
    std::string configFile;
    while ((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            configFile = optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    // 开始加载配置文件
    config_.LoadConfigFile(configFile.c_str());
}

MprpcApplication &MprpcApplication::getInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig &MprpcApplication::getMprpcConfig()
{
    return config_;
    // TODO: 在此处插入 return 语句
}
