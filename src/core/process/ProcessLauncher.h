#pragma once

#include <windows.h>

#include <string>

class ProcessLauncher
{
public:
    ProcessLauncher() = default;

    PROCESS_INFORMATION launchSuspended(
        const std::wstring& programPath
    ) const;

    void resume(PROCESS_INFORMATION& processInfo) const;

    void terminate(PROCESS_INFORMATION& processInfo) const;

    void close(PROCESS_INFORMATION& processInfo) const;

private:
    std::wstring buildCommandLine(
        const std::wstring& programPath
    ) const;
};