#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

// mprpc框架基础类
class MprpcApplication
{
public:
    static void init(int argc, char **argv);
    static MprpcApplication& getInstance();
    static MprpcConfig &getMprpcConfig();

private:
    static MprpcConfig config_;

    MprpcApplication(){}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
};

#endif // MPRPCAPPLICATION_H