#include "user.pb.h"
#include "mprpcchannel.h"
// #include "mprpcapplication.h"
#include <mprpcapplication.h>

int main(int argc, char** argv)
{
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel);
    fixbug::LoginRequest requset;
    requset.set_name("zhangsan");
    requset.set_pwd("12345");

    fixbug::LoginResponse response;
    stub.Login(nullptr, &requset, &response, nullptr);


    
    // if (response.result().errcode() == 0)
    // {
    //     std::cout << "Login success "<< response.success() << std::endl;
    // } else {
    //     std::cout << "Login failed "<< response.result().errmsg() << std::endl;
    // }
    std::cout << " response.result().errmsg():"<< response.result().errmsg() << std::endl;
    std::cout << " response.result().errcode():"<< response.result().errcode() << std::endl;
    std::cout << " response.success():"<< response.success() << std::endl;

}