#include "mprpccontroller.h"

void MprpcController::Reset() 
{ }

bool MprpcController::Failed() const
{
    return m_Failed;
}

std::string MprpcController::ErrorText() const 
{
    return m_ErrorText;
}

void MprpcController::StartCancel() 
{ }

void MprpcController::SetFailed(const std::string& reason) 
{
    m_Failed = true;
    m_ErrorText = reason;
}

bool MprpcController::IsCanceled() const 
{
    return false;
}

void MprpcController::NotifyOnCancel(google::protobuf::Closure* callback) 
{ }