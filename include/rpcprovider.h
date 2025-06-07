#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H

#include "google/protobuf/service.h"
#include "mprpcapplication.h"
#include "google/protobuf/descriptor.h"
#include "zookeeperutil.h"

#include <string>
#include <functional>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>


// 框架提供的专门服务发布rpc服务的网络对象
class RpcProvider
{
public:
    // 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程调用服务
    void run();

private:
    muduo::net::EventLoop eventLoop_;
    // 服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service *serviceName_;    // 保存服务对象
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor *> methodMap_;    // 保存服务方法
    };
    // 存储注册成功的服务对象和其服务方法的所有信息
    std::unordered_map<std::string, ServiceInfo> serviceMap_;

    void onConnection(const muduo::net::TcpConnectionPtr &);
    void onMessage(const muduo::net::TcpConnectionPtr &, muduo::net::Buffer *, muduo::Timestamp);
    // closure的回调函数操作， 用于序列化rpc的响应和网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr &, google::protobuf::Message *);
};

#endif // RPCPROVIDER_H