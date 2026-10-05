#include "ResourceSession.h"

#include <stdexcept>

ResourceSession::ResourceSession(
    const ResourceConfig& config
)
    : config_(config),
    state_(ResourceSessionState::Created)
{}

ResourceSession::~ResourceSession()
{
    if (state_ == ResourceSessionState::Running)
    {
        terminate();
    }

    if (processInfo_.hProcess != nullptr ||
        processInfo_.hThread != nullptr)
    {
        launcher_.close(processInfo_);
    }
}

void ResourceSession::start(
    const std::wstring& programPath
)
{
    if (state_ != ResourceSessionState::Created)
    {
        throw std::runtime_error(
            "Resource session cannot be started."
        );
    }

    job_.apply(config_);

    processInfo_ =
        launcher_.launchSuspended(programPath);

    try
    {
        if (!AssignProcessToJobObject(
            job_.handle(),
            processInfo_.hProcess))
        {
            throw std::runtime_error(
                "Failed to assign process to Job Object."
            );
        }

        launcher_.resume(processInfo_);

        state_ = ResourceSessionState::Running;
    }
    catch (...)
    {
        launcher_.terminate(processInfo_);
        launcher_.close(processInfo_);

        throw;
    }
}

void ResourceSession::update()
{
    if (state_ != ResourceSessionState::Running)
    {
        return;
    }

    const DWORD result =
        WaitForSingleObject(
            processInfo_.hProcess,
            0
        );

    if (result == WAIT_OBJECT_0)
    {
        state_ = ResourceSessionState::Finished;
    }
    else if (result == WAIT_FAILED)
    {
        throw std::runtime_error(
            "Failed to query process state."
        );
    }
}

ResourceSessionState ResourceSession::state() const
{
    return state_;
}

DWORD ResourceSession::processId() const
{
    return processInfo_.dwProcessId;
}

HANDLE ResourceSession::processHandle() const
{
    return processInfo_.hProcess;
}

void ResourceSession::terminate()
{
    if (state_ != ResourceSessionState::Running)
    {
        return;
    }

    launcher_.terminate(processInfo_);
    launcher_.close(processInfo_);

    state_ = ResourceSessionState::Terminated;
}