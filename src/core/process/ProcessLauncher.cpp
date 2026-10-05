#include "ProcessLauncher.h"

#include <stdexcept>

std::wstring ProcessLauncher::buildCommandLine(
    const std::wstring& programPath
) const
{
    return L"\"" + programPath + L"\"";
}

PROCESS_INFORMATION ProcessLauncher::launchSuspended(
    const std::wstring& programPath
) const
{
    if (programPath.empty())
    {
        throw std::invalid_argument(
            "Program path cannot be empty."
        );
    }

    std::wstring commandLine =
        buildCommandLine(programPath);

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};

    if (!CreateProcessW(
        nullptr,
        &commandLine[0],
        nullptr,
        nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo))
    {
        throw std::runtime_error(
            "Failed to create process."
        );
    }

    return processInfo;
}

void ProcessLauncher::resume(
    PROCESS_INFORMATION& processInfo
) const
{
    if (ResumeThread(processInfo.hThread) ==
        static_cast<DWORD>(-1))
    {
        throw std::runtime_error(
            "Failed to resume process."
        );
    }
}

void ProcessLauncher::terminate(
    PROCESS_INFORMATION& processInfo
) const
{
    if (processInfo.hProcess != nullptr)
    {
        TerminateProcess(
            processInfo.hProcess,
            1
        );
    }
}

void ProcessLauncher::close(
    PROCESS_INFORMATION& processInfo
) const
{
    if (processInfo.hThread != nullptr)
    {
        CloseHandle(processInfo.hThread);
        processInfo.hThread = nullptr;
    }

    if (processInfo.hProcess != nullptr)
    {
        CloseHandle(processInfo.hProcess);
        processInfo.hProcess = nullptr;
    }
}