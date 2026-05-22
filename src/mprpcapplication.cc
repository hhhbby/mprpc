#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::m_config;

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

void showArgsHelpAndExit()
{
    std::cout << "format: command -i <config file>" << std::endl;
    exit(EXIT_FAILURE);
}

void MprpcApplication::Init(int argc, char **argv)
{
    if (argc < 2)
        showArgsHelpAndExit();


    int opt;
    std::string configFile;
    // 首字符为 ':' 表示区分"缺少参数"和"未知选项"，当没有前置':'时且缺少参数时，getopt 会返回 '?'，而不是 ':'
    while ((opt = getopt(argc, argv, "i:")) != -1)
    {

        switch (opt)
        {
        case 'i':
            configFile = optarg;
            break;
        default:
            showArgsHelpAndExit();
        }
    }
    m_config.LoadConfigFile(configFile.c_str());

    std::cout << "rpcserverip : " << m_config.Load("rpcserverip") << std::endl;
    std::cout << "rpcserverport : " << m_config.Load("rpcserverport") << std::endl;
    std::cout << "zookeeperserverip : " << m_config.Load("zookeeperserverip") << std::endl;
    std::cout << "zookeeperserverport : " << m_config.Load("zookeeperserverport") << std::endl;
    
}

MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}