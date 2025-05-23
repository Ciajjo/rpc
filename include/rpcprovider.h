#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H

#include "google/protobuf/service.h"

// 框架提供的专门服务发布rpc服务的网络对象
class RpcProvider
{
public:
    RpcProvider();
    ~RpcProvider();
    // 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service *service);

    // 启动rpc服务节点，开始提供rpc远程调用服务
    void run();

private:

};

#endif // RPCPROVIDER_H