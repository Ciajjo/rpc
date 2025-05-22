#include "mprpcapplication.h"

void MprpcApplication::init(int argc, char **argv)
{
}

MprpcApplication& MprpcApplication::getInstance()
{
    static MprpcApplication app;
    return app;
}