#include "zookeeperclient.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>

void g_watcher(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT && state == ZOO_CONNECTED_STATE)
    {
        sem_t* sem = const_cast<sem_t*>(reinterpret_cast<const sem_t*>(zoo_get_context(zh)));
        sem_post(sem);
    }
}

ZookeeperClient::~ZookeeperClient()
{
    if (m_zhandle)
    {
        zookeeper_close(m_zhandle);
    }
}

void ZookeeperClient::start()
{
    std::string ip = MprpcApplication::GetConfig().Load("zookeeperserverip");
    std::string port = MprpcApplication::GetConfig().Load("zookeeperserverport");
    std::string host = ip + ":" + port;

    sem_t sem;
    if (-1 == sem_init(&sem, 0, 0))
    {
        std::cout << "sem_init failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    // connect to zookeeper server async
    m_zhandle = zookeeper_init(host.c_str(), g_watcher, 30000, nullptr, &sem, 0);
    if (!m_zhandle)
    {
        std::cout << "zookeeper_init failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    sem_wait(&sem);

    std::cout << "zookeeper client start success" << std::endl;
}

void ZookeeperClient::create(const char *path, const char *value, int valuelen, int state)
{
    int flag = zoo_exists(m_zhandle, path, 0, nullptr);
    if (flag == ZNONODE)
    {
        flag = zoo_create(m_zhandle, path, value, valuelen, &ZOO_OPEN_ACL_UNSAFE, state, nullptr, 0);
        if (ZOK != flag)
        {
            std::cout << "zoo_create failed, path: " << path << std::endl;
            exit(EXIT_FAILURE);
        } else {
            std::cout << "zoo_create succcess, path: " << path << std::endl;
        }
    } else {
        std::cout << "node " << path << " alread exists" << std::endl;
    }
}

std::string ZookeeperClient::get(const char* path)
{
    char buf[128] = {0};
    int bufLen = sizeof(buf);
    int flag = zoo_get(m_zhandle, path, 0, buf, &bufLen, nullptr);
    if (ZOK != flag)
    {
        std::cout << "zoo_get failed, path: " << path << std::endl;
        exit(EXIT_FAILURE);
    }
    return buf;
}