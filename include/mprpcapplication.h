#ifndef MPRPCAPPLICATION_H
#define MPRPCAPPLICATION_H

// mprpc框架基础类
class MprpcApplication
{
public:
    static void init(int argc, char **argv);
    static MprpcApplication& getInstance();

private:
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication &) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
};

#endif // MPRPCAPPLICATION_H