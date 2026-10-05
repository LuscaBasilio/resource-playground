#include "../../src/core/monitor/ResourceMonitor.h"

#include <windows.h>

#include <cstdlib>
#include <iostream>

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 2)
    {
        std::wcout
            << L"Usage:\n"
            << L"  ResourceMonitor.exe <program.exe>\n";

        return EXIT_FAILURE;
    }

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};

    std::wstring commandLine =
        L"\"" + std::wstring(argv[1]) + L"\"";

    if (!CreateProcessW(
        nullptr,
        &commandLine[0],
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo))
    {
        std::wcerr
            << L"CreateProcessW failed. Error: "
            << GetLastError()
            << L'\n';

        return EXIT_FAILURE;
    }

    CloseHandle(processInfo.hThread);

    try
    {
        ResourceMonitor monitor(
            processInfo.hProcess
        );

        while (true)
        {
            const DWORD waitResult =
                WaitForSingleObject(
                    processInfo.hProcess,
                    500
                );

            const ResourceMetrics metrics =
                monitor.sample();

            std::wcout
                << L"CPU: "
                << metrics.cpu.usagePercent
                << L"%\n";

            if (waitResult == WAIT_OBJECT_0)
            {
                break;
            }

            if (waitResult == WAIT_FAILED)
            {
                break;
            }
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        TerminateProcess(
            processInfo.hProcess,
            1
        );

        CloseHandle(processInfo.hProcess);

        return EXIT_FAILURE;
    }

    CloseHandle(processInfo.hProcess);

    return EXIT_SUCCESS;
}