#include <iostream>
#include <string>

#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

/**
 * userservice原来是一个本地服务，提供两个进程的本地方法，Login和GetFriendLiset
 */
class UserSerivce : public fixbug::UserServiceRpc // 使用在rpc发布端（rpc服务提供者）
{
public:
    bool Login(std::string name, std::string pwd);
    // 重写UserServiceRpc基类的虚函数
    void Login(google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done);
};

bool UserSerivce::Login(std::string name, std::string pwd)
{
    std::cout << "doing local service: Login" << std::endl;
    std::cout << "name" << name << "\tpwd" << pwd << std::endl;
}

void UserSerivce::Login(google::protobuf::RpcController *controller, const ::fixbug::LoginRequest *request, ::fixbug::LoginResponse *response, ::google::protobuf::Closure *done)
{
    // 框架给业务上报请求参数，获取数据做本地业务
    std::string name = request->name();
    std::string pwd = request->pwd();

    // 做本地业务
    bool loginResult = Login(name, pwd);

    // 作出回应
    fixbug::ResultCode *result = response->mutable_result();
    result->set_errcode(0);
    result->set_errmsg("");
    response->set_success(loginResult);

    // 执行回调函数， 对象数据序列化和网络发送
    done->Run();
}

int main(int argc, char** argv)
{
    MprpcApplication::init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new UserSerivce());
}