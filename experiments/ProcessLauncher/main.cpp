#include "../../src/core/process/ProcessLauncher.h"

#include <windows.h>

#include <cstdlib>
#include <iostream>

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 2)
    {
        std::wcout
            << L"Usage:\n"
            << L"  ProcessLauncher.exe <program.exe>\n";

        return EXIT_FAILURE;
    }

    const std::wstring programPath = argv[1];

    ProcessLauncher launcher;

    PROCESS_INFORMATION processInfo{};

    try
    {
        processInfo =
            launcher.launchSuspended(programPath);

        std::wcout
            << L"Process created successfully.\n"
            << L"PID: "
            << processInfo.dwProcessId
            << L'\n';

        std::wcout
            << L"Process is suspended.\n";

        launcher.resume(processInfo);

        std::wcout
            << L"Process resumed.\n"
            << L"Waiting for process to finish...\n";

        const DWORD waitResult =
            WaitForSingleObject(
                processInfo.hProcess,
                INFINITE
            );

        if (waitResult == WAIT_FAILED)
        {
            std::wcerr
                << L"WaitForSingleObject failed. "
                << L"GetLastError() = "
                << GetLastError()
                << L'\n';

            launcher.close(processInfo);

            return EXIT_FAILURE;
        }

        DWORD exitCode = 0;

        if (GetExitCodeProcess(
            processInfo.hProcess,
            &exitCode))
        {
            std::wcout
                << L"Process finished.\n"
                << L"Exit code: "
                << exitCode
                << L'\n';
        }

        launcher.close(processInfo);
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        if (processInfo.hProcess != nullptr)
        {
            launcher.terminate(processInfo);
            launcher.close(processInfo);
        }

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}