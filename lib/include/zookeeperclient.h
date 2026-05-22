#include <zookeeper/zookeeper.h>
#include <string>

class ZookeeperClient
{
public:
    ~ZookeeperClient();
    void start();

    void create(const char *path, const char *value, int valuelen, int state = 0);

    std::string get(const char* path);
private:
    zhandle_t *m_zhandle = nullptr;
};