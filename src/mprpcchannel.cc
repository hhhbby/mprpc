#include <mprpcchannel.h>
#include "rpcheader.pb.h"
#include "mprpcapplication.h"
#include "zookeeperclient.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>


void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor *service = method->service();
    
    std::string args_str;
    if (!request->SerializeToString(&args_str))
    {
        controller->SetFailed("request serialize failed");
        return;
    }

    std::string service_name = service->name();
    std::string method_name = method->name();
    uint32_t args_size = args_str.size();

    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);

    std::string rpc_header_str;
    if (!rpc_header.SerializeToString(&rpc_header_str))
    {
        controller->SetFailed("rpc header serialize failed");
        return;
    }
    uint32_t header_size = rpc_header_str.size();

    // headersize + rpc header  request
    std::string send_str;
    send_str.insert(0, reinterpret_cast<const char*>(&header_size), 4);
    send_str = send_str + rpc_header_str + args_str;

    std::cout << "==================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "==================================================" << std::endl;

    // 接下来将其发送到服务端
    // std::string ip(MprpcApplication::GetConfig().Load("rpcserverip"));
    // uint16_t port(atoi(MprpcApplication::GetConfig().Load("rpcserverport").c_str()));

    ZookeeperClient zkclient;
    zkclient.start();
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host =zkclient.get(method_path.c_str());
    int idx = host.find(':');
    if (idx == std::string::npos)
    {
        controller->SetFailed("zookeeper get failed, methode_path: " + method_path + ", host: " + host);
        return;
    }
    std::string ip = host.substr(0, idx);
    std::string port_str = host.substr(idx + 1, host.size() - idx - 1);
    uint16_t port = atoi(port_str.c_str());

    int sock;
    if (-1 == (sock = socket(AF_INET, SOCK_STREAM, 0)))
    {
        controller->SetFailed("create sock failed, errno: " + std::to_string(errno));
        return;
    }
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (-1 == inet_pton(AF_INET, ip.c_str(), &addr.sin_addr))
    {
        controller->SetFailed("inet_pton failed, Invalid address/ Address not supported");
        close(sock);
        return;
    }

    if (-1 == connect(sock, reinterpret_cast<sockaddr*>(&addr), static_cast<socklen_t>(sizeof(addr))))
    {
        controller->SetFailed("connect failed, errno: " + std::to_string(errno));
        close(sock);
        return;
    }

    if (-1 == send(sock, send_str.c_str(), send_str.size(), 0))
    {
        controller->SetFailed("send failed, errno: " + std::to_string(errno));
        close(sock);
        return;
    }
    
    char buf[1024] = {0};
    int recv_size = 0;

    /*
    // test controller
    controller->SetFailed("recv failed, errno: " + std::to_string(errno));
    close(sock);
    return;
    */
    if (-1 == (recv_size = recv(sock, buf, sizeof(buf), 0)))
    {
        controller->SetFailed("recv failed, errno: " + std::to_string(errno));
        close(sock);
        return;
    }

    if (!response->ParseFromArray(buf, recv_size))
    {
        controller->SetFailed("response parse failed" + std::to_string(errno));
        close(sock);
        return;
    }
    
    close(sock);
}                        