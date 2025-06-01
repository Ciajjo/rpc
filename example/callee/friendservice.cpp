#include <iostream>
#include <string>
#include <vector>

#include "friend.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class FriendService : public fixbug::GetFriends
{
public:
    std::vector<std::string> GetFriendsList(uint32_t userid);
    void GetFriendsList(::google::protobuf::RpcController *controller,
                        const ::fixbug::GetFriendsRequest *request,
                        ::fixbug::GetFriendsResponse *response,
                        ::google::protobuf::Closure *done);

private:
};

std::vector<std::string> FriendService::GetFriendsList(uint32_t userid)
{
    std::cout << "GetFriendsList service!" << std::endl;
    std::vector<std::string> vec;
    vec.push_back("gggg");
    vec.push_back("uuuu");
    vec.push_back("jjjj");
    return vec;
}

void FriendService::GetFriendsList(::google::protobuf::RpcController *controller, const ::fixbug::GetFriendsRequest *request, ::fixbug::GetFriendsResponse *response, ::google::protobuf::Closure *done)
{
    uint32_t userid = request->userid();
    std::vector<std::string> friendsList = GetFriendsList(userid);
    response->mutable_result()->set_errcode(0);
    response->mutable_result()->set_errmsg("");
    for (std::string &name : friendsList)
    {
        std::string *p = response->add_friends();
        *p = name;
    }
    done->Run();
}

int main(int argc, char **argv)
{

    // 调用框架的初始化操作
    MprpcApplication::init(argc, argv);

    // provider是一个rpc网络服务对象。把UserService对象发布到rpc节点上
    RpcProvider provider;
    provider.NotifyService(new FriendService());

    // 启动一个rpc服务发布节点   Run以后，进程进入阻塞状态，等待远程的rpc调用请求
    provider.run();

    return 0;
}
