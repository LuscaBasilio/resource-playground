#pragma once

#include "../job/JobObject.h"
#include "../process/ProcessLauncher.h"
#include "../resource/ResourceConfig.h"

#include <windows.h>

#include <string>

enum class ResourceSessionState
{
    Created,
    Running,
    Finished,
    Terminated
};

class ResourceSession
{
public:
    explicit ResourceSession(
        const ResourceConfig& config
    );

    ~ResourceSession();

    ResourceSession(const ResourceSession&) = delete;
    ResourceSession& operator=(const ResourceSession&) = delete;

    void start(const std::wstring& programPath);

    void update();

    ResourceSessionState state() const;

    DWORD processId() const;

    HANDLE processHandle() const;

    void terminate();

private:
    ResourceConfig config_;

    JobObject job_;
    ProcessLauncher launcher_;

    PROCESS_INFORMATION processInfo_{};

    ResourceSessionState state_;
};