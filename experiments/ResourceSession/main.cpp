#include "../../src/core/session/ResourceSession.h"

#include <windows.h>

#include <cstdlib>
#include <iostream>

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::wcout
            << L"Usage:\n"
            << L"  ResourceSession.exe <program.exe> [cpu_percent]\n";

        return EXIT_FAILURE;
    }

    const std::wstring programPath = argv[1];

    ResourceConfig config;

    if (argc == 3)
    {
        const int cpuPercent =
            std::stoi(argv[2]);

        if (cpuPercent < 1 || cpuPercent > 100)
        {
            std::wcerr
                << L"CPU percentage must be between 1 and 100.\n";

            return EXIT_FAILURE;
        }

        CpuConfig cpuConfig;

        cpuConfig.percent =
            static_cast<unsigned int>(cpuPercent);

        cpuConfig.affinityMask = 0b0011;

        config.cpu = cpuConfig;
    }

    try
    {
        ResourceSession session(config);

        session.start(programPath);

        std::wcout
            << L"Process started.\n"
            << L"PID: "
            << session.processId()
            << L'\n';

        while (true)
        {
            session.update();

            if (session.state() !=
                ResourceSessionState::Running)
            {
                break;
            }

            Sleep(500);
        }

        if (session.state() ==
            ResourceSessionState::Finished)
        {
            std::wcout
                << L"Process finished normally.\n";
        }
        else if (session.state() ==
            ResourceSessionState::Terminated)
        {
            std::wcout
                << L"Process was terminated.\n";
        }
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Error: "
            << exception.what()
            << '\n';

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}