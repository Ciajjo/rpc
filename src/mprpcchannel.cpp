#include "mprpcchannel.h"
#include "rpcprovider.pb.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "zookeeperutil.h"

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
        controller->SetFailed("serialize request error");
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
        controller->SetFailed("serialize error");
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
        char errtxt[512] = {0};
        sprintf(errtxt, "create socket error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // std::string ip = MprpcApplication::getInstance().getMprpcConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::getInstance().getMprpcConfig().Load("rpcserverport").c_str());

    // 读取配置文件rpcserver的信息
    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    // rpc调用方想调用service_name的method_name服务，需要查询zk上该服务所在的host信息
    ZkClient zkCli;
    zkCli.Start();
    //  /UserServiceRpc/Login
    std::string method_path = "/" + serviceName + "/" + methodName;
    // 127.0.0.1:8000
    std::string host_data = zkCli.GetData(method_path.c_str());
    if (host_data == "")
    {
        controller->SetFailed(method_path + " is not exist!");
        return;
    }
    int idx = host_data.find(":");
    if (idx == -1)
    {
        controller->SetFailed(method_path + " address is invalid!");
        return;
    }
    std::string ip = host_data.substr(0, idx);
    uint16_t port = atoi(host_data.substr(idx + 1, host_data.size() - idx).c_str());

    sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 连接rpc服务节点
    if (connect(cfd, reinterpret_cast<sockaddr *>(&saddr), sizeof(saddr)) == -1)
    {
        close(cfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "connect error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    if (send(cfd, rpcStr.c_str(), rpcStr.size(), 0) == -1)
    {
        close(cfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "send error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 接受rpc请求的响应值
    char recvBuf[BUFSIZ] = {0};
    int recvSize = 0;
    if ((recvSize = recv(cfd, recvBuf, BUFSIZ, 0)) == -1)
    {
        close(cfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "recv error! errno:%d", errno);
        controller->SetFailed(errtxt);
        return;
    }

    // 反序列化rpc响应调用的响应数据
    // std::string responseStr(recvBuf, 0, recvSize);
    // if (!response->ParseFromString(responseStr))
    if (!response->ParseFromArray(recvBuf, recvSize))
    {
        close(cfd);
        char errtxt[512] = {0};
        sprintf(errtxt, "parse error! response_str:%s", recvBuf);
        controller->SetFailed(errtxt);
        return;
    }
}
