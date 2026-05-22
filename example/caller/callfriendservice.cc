#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"
#include "friend.pb.h"

int main(int argc, char** argv)
{
    MprpcApplication::Init(argc, argv);
    fixbug::GetFriendListRequest request;
    fixbug::GetFriendListResponse response;
    request.set_id(13414);

    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel);
    MprpcController controller;
    stub.GetFriendList(&controller, &request, &response, nullptr);


    if (controller.Failed())   
    {
        std::cout << "rpc failed, because " << controller.ErrorText() << std::endl;
        return 1;
    }

    std::cout << "response.result().errmsg():"<< response.result().errmsg() << std::endl;
    std::cout << "response.result().errcode():"<< response.result().errcode() << std::endl;
    int sz = response.friend_list_size();
    for (int i = 0; i < sz; ++i)
    {
        std::cout << "idx: " << i << ", name: " << response.friend_list(i) << std::endl;
    }

}