#include <iostream>

#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"

int main(int argc, char **argv)
{
    // 整个程序启动以后，想用mprpc框架来享受rpc的服务调用，一定要先调用框架的初始化函数（只初始化一次）
    MprpcApplication::init(argc, argv);

    // 演示调用远程发布的rpc的方法login
    fixbug::UserServiceRpc_Stub stub(new MpRpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;
    stub.Login(nullptr, &request, &response, nullptr); //

    if (response.result().errcode() == 0)
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }

    // 演示调用远程发布的rpc的方法login
    fixbug::RegisterRequest registerRequest;
    registerRequest.set_id(1);
    registerRequest.set_name("zhang san");
    registerRequest.set_pwd("123456");

    fixbug::RegisterResponse registerResponse;
    stub.Register(nullptr, &registerRequest, &registerResponse, nullptr);
    if (registerResponse.result().errcode() == 0)
    {
        std::cout << "rpc login response success: " << response.success() << std::endl;
    }
    else
    {
        std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
    }
}