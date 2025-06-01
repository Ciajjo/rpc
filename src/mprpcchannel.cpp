#include "mprpcchannel.h"
#include "rpcprovider.pb.h"
#include <mprpcapplication.h>

#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <error.h>

void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response, google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string serviceName = sd->name();
    std::string methodName = method->name();

    // 获取参数的序列化字符串的长度
    std::string argsStr;
    int32_t argsSize;
    if (request->SerializeToString(&argsStr))
    {
        argsSize = argsStr.size();
    }
    else
    {
        std::cout << " serialize error" << std::endl;
        return;
    }

    // 定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_servicename(serviceName);
    rpcHeader.set_methodname(methodName);
    rpcHeader.set_argcsize(argsSize);

    std::string rpcHeaderStr;
    int32_t rpcHeaderSize;
    if (rpcHeader.SerializeToString(&rpcHeaderStr))
    {
        rpcHeaderSize = rpcHeaderStr.size();
    }
    else
    {
        std::cout << " serialize error" << std::endl;
        return;
    }

    // 组织待发送的rpc请求的字符串
    std::string rpcStr;
    rpcStr.insert(0, reinterpret_cast<char *>(&rpcHeaderSize), 4);
    rpcStr += rpcHeaderStr;
    rpcStr += argsStr;

    std::cout << "=======================" << std::endl;
    std::cout << "rpcHeadStr" << rpcHeaderStr << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "headerSize" << rpcHeaderSize << std::endl;
    std::cout << "serviceName" << serviceName << std::endl;
    std::cout << "methodName" << methodName << std::endl;
    std::cout << "argsStr" << argsStr << std::endl;
    std::cout << "=======================" << std::endl;

    int cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1)
    {
        std::cout << "socket error " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string ip = MprpcApplication::getInstance().getMprpcConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::getInstance().getMprpcConfig().Load("rpcserverport").c_str());

    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (connect(cfd, reinterpret_cast<sockaddr *>(&saddr), sizeof (saddr)) == -1)
    {
        std::cout << "connect error " << errno << std::endl;
        close(cfd);
        exit(EXIT_FAILURE);
    }

    if (send(cfd, rpcStr.c_str(), rpcStr.size(), 0) == -1)
    {
        std::cout << "send error " << errno << std::endl;
        close(cfd);
        return;
    }

    // 接受rpc请求的响应值
    char recvBuf[BUFSIZ] = {0};
    int recvSize = 0;
    if ((recvSize = recv(cfd, recvBuf, BUFSIZ, 0)) == -1)
    {
        std::cout << "send error " << errno << std::endl;
        close(cfd);
        return;
    }

    // 反序列化rpc响应调用的响应数据
    // std::string responseStr(recvBuf, 0, recvSize);
    // if (!response->ParseFromString(responseStr))
    if (!response->ParseFromArray(recvBuf, recvSize))
    {
        std::cout << "parse error recvBuf:  " << recvBuf << std::endl;
        close(cfd);
        return;
    }
}
