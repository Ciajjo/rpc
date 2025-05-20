#include "test.pb.h"

#include <iostream>
#include <string>

using namespace fixbug;

int main()
{
    LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");

    std::string strreq;
    if (req.SerializeToString(&strreq))
    {
        std::cout << strreq.c_str() << std::endl;
    }

    LoginRequest B;
    if (B.ParseFromString(strreq))
    {
        std::cout << B.name() << std::endl;
        std::cout << B.pwd() << std::endl;
    }
}