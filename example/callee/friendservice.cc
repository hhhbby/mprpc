#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "friend.pb.h"
#include <vector>
#include <string>

class FriendService : public fixbug::FriendServiceRpc
{
public:
    std::vector<std::string> GetFriendList(uint32_t user_id)
    {
        std::cout << "doing GetFriendList Service, user_id = " << user_id << std::endl;
        std::vector<std::string> list;
        list.push_back("sh");
        list.push_back("pzy");
        list.push_back("fyb");
        return list;
    }

    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        uint32_t user_id = request->id();
        std::vector<std::string> list = GetFriendList(user_id);


        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for (const std::string& name : list)
        {
            std::string *ps = response->add_friend_list();
            *ps = name;
        }
        done->Run();
    }
};


int main(int argc, char** argv)
{
    MprpcApplication::Init(argc, argv);
    RpcProvider provider;
    provider.NotifyService(new FriendService);
    provider.Run();
}