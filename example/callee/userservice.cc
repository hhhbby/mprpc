#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

class UserService : public fixbug::UserServiceRpc {
public:
    bool Login(const std::string &name, const std::string &pwd)
    {
        std::cout << "Login:" << std::endl;
        std::cout << "name : " << name << std::endl;
        std::cout << "pwd : " << pwd << std::endl;
        return false;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        response->set_success(Login(request->name(), request->pwd()));

        fixbug::ResultCode *rc = response->mutable_result();
        rc->set_errcode(3);
        rc->set_errmsg("server error");

        done->Run();
    }

};

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new UserService);
    provider.Run();
    return 0;

}