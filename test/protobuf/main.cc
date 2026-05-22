#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    GetFriendListResponse rsp;
    ResultCode* c = rsp.mutable_res();
    c->set_errcode(0);

    User* u = rsp.add_friend_list();
    u->set_name("huang");
    u->set_age(12);
    u->set_gender(User::MAN);

    User* u2 = rsp.add_friend_list();
    u2->set_name("huang");
    u2->set_age(121);
    u2->set_gender(User::MAN);

    std::cout << rsp.friend_list_size() << std::endl;
    std::cout << rsp.SerializeAsString() << std::endl;
    return 0;
}

int main1()
{
    LoginRequest req;
    req.set_name("hby");
    req.set_pwd("1234");
    std::string send_str;
    if (req.SerializeToString(&send_str))
        std::cout << send_str << std::endl;


    LoginRequest req2;
    if (req2.ParseFromString(send_str))
    {
        std::cout << req2.name() << std::endl;
        std::cout << req2.pwd() << std::endl;
    }
    return 0;
}