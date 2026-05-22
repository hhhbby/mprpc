protoc test.proto --cpp_out=.
g++ test.pb.cc main.cc -lprotobuf
./a.out