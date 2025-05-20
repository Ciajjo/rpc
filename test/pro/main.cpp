#include "test.pb.h"

#include <iostream>
#include <string>

using namespace fixbug;

void t1()
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

void t2()
{
    LoginResponse rsp;
    ResultCode *rc = rsp.mutable_result();
    rc->set_errcode(1);
    rc->set_errmsg("Fffff");

    std::string str;
    rsp.SerializeToString(&str);
    std::cout << str.c_str() << std::endl;

    LoginResponse b;
    b.ParseFromString(str);
    std::cout << b.result().errcode() << std::endl;
    std::cout << b.result().errmsg() << std::endl;
}

void t3()
{
    GetfriendListResponse f;
    ResultCode *rp = f.mutable_resultcode();
    rp->set_errcode(0);
    User *up = f.add_friendlist();
    up->set_name("llll");
    up->set_age(20);
    up->set_sex(User::MAN);

    User *u1 = f.add_friendlist();
    u1->set_name("llll");
    u1->set_age(20);
    u1->set_sex(User::MAN);

    std::cout << f.friendlist_size() << std::endl;
}
int main()
{
    t3();
}
