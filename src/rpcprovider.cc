#include "rpcprovider.h"
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpServer.h>
#include <google/protobuf/descriptor.h>
#include "mprpcapplication.h"
#include "rpcheader.pb.h"

RpcProvider::RpcProvider()
{
    m_zkclient.start();
}

void RpcProvider::NotifyService(google::protobuf::Service *s)
{
    const google::protobuf::ServiceDescriptor *psd = s->GetDescriptor();
    std::string sn = psd->name();
    int methodCnt = psd->method_count();

    ServerInfo si;
    si.m_service = s;
    std::cout << "service_name:" << sn<< std::endl;

    std::string service_path = "/" + sn;
    m_zkclient.create(service_path.c_str(), nullptr, -1);

    std::string ip = MprpcApplication::GetConfig().Load("rpcserverip");
    std::string port = MprpcApplication::GetConfig().Load("rpcserverport");
    std::string host = ip + ":" + port;

    for (int i = 0; i < methodCnt; ++i)
    {
        const google::protobuf::MethodDescriptor *pmd = psd->method(i);
        std::string method_name = pmd->name();
        si.m_methodMap.insert({pmd->name(), pmd});

        std::string method_path = service_path + "/" + method_name;
        m_zkclient.create(method_path.c_str(), host.c_str(), host.size(),ZOO_EPHEMERAL);
        
        std::cout << "\tmethod_name:" << method_name << std::endl;
    }
    m_serviceMap.insert({sn, si});
}

void RpcProvider::Run()
{
    std::string ip(MprpcApplication::GetConfig().Load("rpcserverip"));
    uint16_t port(atoi(MprpcApplication::GetConfig().Load("rpcserverport").c_str()));

    muduo::net::InetAddress addr(ip, port);
    muduo::net::TcpServer server(&m_loop, addr, "RpcProvider");

    server.setConnectionCallback(std::bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage, this, std::placeholders::_1, 
                std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);
    std::cout << "RpcProvide start service at  " << ip << ":" << port << std::endl;
    server.start();
    m_loop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (!conn->connected())
        conn->shutdown();
}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,
                        muduo::net::Buffer* buf,
                        muduo::Timestamp)
{
    std::string recv_buf = buf->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_buf.copy(reinterpret_cast<char*>(&header_size), 4);
    std::string rpc_header_str = recv_buf.substr(4, header_size);

    mprpc::RpcHeader rpc_header;
    if (!rpc_header.ParseFromString(rpc_header_str))
    {
        std::cout << "parse RpcHeader failed" << std::endl;
        return;
    }

    std::string service_name = rpc_header.service_name();
    auto sit = m_serviceMap.find(service_name);
    if (sit == m_serviceMap.end())
    {
        std::cout << service_name << " is not exist" << std::endl;
        return;
    }
    std::string method_name = rpc_header.method_name();
    google::protobuf::Service *service = sit->second.m_service;
    auto mit = sit->second.m_methodMap.find(method_name);
    if (mit == sit->second.m_methodMap.end())
    {
        std::cout << service_name << " : " << method_name << " is not exist" << std::endl;
        return;
    }
    const google::protobuf::MethodDescriptor *method_desc = mit->second;


    uint32_t args_size = rpc_header.args_size();
    std::string args_str = recv_buf.substr(header_size + 4, args_size);

    std::cout << "==================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "==================================================" << std::endl;
    
    google::protobuf::Message *request = service->GetRequestPrototype(method_desc).New();
    if (!request->ParseFromString(args_str))
    {
        std::cout << "parse Message request failed" << std::endl;
        return;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method_desc).New();

    google::protobuf::Closure* done = google::protobuf::NewCallback
                <RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
                (this, &RpcProvider::sendRpcResponse, conn, response);

    service->CallMethod(method_desc, nullptr, request, response, done);
    
}

void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response)
{
    std::string send_str;
    if (!response->SerializeToString(&send_str))
    {
        std::cout << "response serialize failed" << std::endl;
    } else {
        conn->send(send_str);
    }
    conn->shutdown();
}