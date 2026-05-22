#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/EventLoop.h>
#include <muduo/base/Timestamp.h>
#include <unordered_map>
#include "zookeeperclient.h"

class RpcProvider
{
public:
    RpcProvider();
    void NotifyService(google::protobuf::Service *s);
    void Run();
private:

    struct ServerInfo {
        google::protobuf::Service *m_service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> m_methodMap;
    };
    std::unordered_map<std::string, ServerInfo> m_serviceMap;
    ZookeeperClient m_zkclient;
    

    muduo::net::EventLoop m_loop;
    void onConnection(const muduo::net::TcpConnectionPtr&);
    void onMessage(const muduo::net::TcpConnectionPtr&,
                            muduo::net::Buffer*,
                            muduo::Timestamp);
    void sendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message *);
};