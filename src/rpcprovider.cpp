#include "rpcprovider.h"
#include "rpcprovider.pb.h"

void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo serviceInfo;
    serviceInfo.serviceName_ = service;

    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *p = service->GetDescriptor();
    // 获取服务的名字
    const std::string serviceName = p->name();
    // 获取服务方法数量
    int methodCount = p->method_count();

    std::cout << "servicename: " << serviceName << std::endl;

    for (int i = 0; i < methodCount; ++i)
    {
        // 获取了服务对象指定的方法的描述
        const google::protobuf::MethodDescriptor *pm = p->method(i);
        std::string methodName = pm->name();
        serviceInfo.methodMap_.emplace(methodName, pm);
        std::cout << "methodName: " << methodName << std::endl;
    }
    serviceMap_.emplace(serviceName, serviceInfo);
}

void RpcProvider::run()
{
    std::string ip = MprpcApplication::getInstance().getMprpcConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::getInstance().getMprpcConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress addr(ip, port);

    // 创建TcpServer对象
    muduo::net::TcpServer server(&eventLoop_, addr, "RpcProvider");
    // 绑定连接回调和消息读写回调
    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    // 设置muduo库的线程数量
    server.setThreadNum(4);

    std::cout << "RpcProvider start at ip:" << ip << "\tport:" << port << std::endl;
    // 启动网络服务
    server.start();
    eventLoop_.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (!conn->connected())
    {
        // 和rpc client断开连接
        conn->shutdown();
    }
}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp time)
{
    std::string recvBuf = buf->retrieveAllAsString();

    uint32_t header_size = 0;
    // 确保 recv_buf 至少有 4 字节数据
    if (recvBuf.size() >= 4)
    {
        memcpy(&header_size, recvBuf.data(), 4); // 正确复制二进制数据
        // 如果需要网络字节序转换（大端 → 主机字节序）：
        // header_size = ntohl(header_size);
    }
    // recvBuf.copy((char *)&header_size, 4, 0);

    std::string rpcHeadStr = recvBuf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string serviceName;
    std::string methodName;
    uint32_t argsSize;
    if (rpcHeader.ParseFromString(rpcHeadStr))
    {
        serviceName = rpcHeader.servicename();
        methodName = rpcHeader.methodname();
        argsSize = rpcHeader.argcsize();
    }
    else
    {
        std::cout << "rpcHeadStr: " << rpcHeadStr << " parse error" << std::endl;
    }
    std::string argsStr = recvBuf.substr(4 + header_size, argsSize);
    std::cout << "=======================" << std::endl;
    std::cout << "rpcHeadStr" << rpcHeadStr << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "headerSize" << header_size << std::endl;
    std::cout << "serviceName" << serviceName << std::endl;
    std::cout << "methodName" << methodName << std::endl;
    std::cout << "argsStr" << argsStr << std::endl;
    std::cout << "=======================" << std::endl;

    // 获取service对象和method对象
    auto it = serviceMap_.find(serviceName);
    if (it == serviceMap_.end())
    {
        std::cout << serviceName << " is not found." << std::endl;
        return;
    }

    auto mit = it->second.methodMap_.find(methodName);
    if (mit == it->second.methodMap_.end())
    {
        std::cout << methodName << " is not found." << std::endl;
        return;
    }

    auto *service = it->second.serviceName_; // 获取对象
    auto *method = mit->second;              //  获取方法描述

    //  生成rpc方法调用的请求request和响应respond参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(argsStr))
    {
        std::cout << "request parse error! content:" << argsStr << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();

    // 给下面的method，绑定一个closure回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr &, google::protobuf::Message *>(this, &RpcProvider::sendRpcResponse, conn, response);

    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string responseStr;
    if (response->SerializeToString(&responseStr)) // 序列化成功后
    {
        // 序列成功后，通过网络吧rpc方法执行的结果发送回rpc的调用方
        conn->send(responseStr);
        conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
    }
    else
    {
        std::cout << "seriallize error" << std::endl;
    }
    conn->shutdown(); // 模拟http的短链接服务，由rpcprovider主动断开连接
}
