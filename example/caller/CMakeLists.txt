# set(SRC_LIST calluserservice.cc ../user.pb.cc)
set(SRC_LIST callfriendservice.cc ../friend.pb.cc)

add_executable(consumer)
target_sources(consumer PRIVATE ${SRC_LIST})
target_link_libraries(consumer mprpc protobuf)