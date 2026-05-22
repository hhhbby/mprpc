#pragma once
#include "mprpcconfig.h"

class MprpcApplication
{
public:
    static MprpcApplication& GetInstance();
    static void Init(int argc, char **argv);
    static MprpcConfig& GetConfig();
private:
    MprpcApplication() = default;
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(const MprpcApplication&&) = delete;

    static MprpcConfig m_config;
};