#include "../../src/core/job/JobObject.h"
#include "../../src/core/process/ProcessLauncher.h"

#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{

    void printLastError(const char* operation)
    {
        const DWORD error = GetLastError();

        std::cerr
            << operation
            << " failed. GetLastError() = "
            << error
            << '\n';
    }

} // namespace

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::cout
            << "Usage:\n"
            << "  JobObject.exe <program.exe> [cpu_percent]\n\n"
            << "Examples:\n"
            << "  JobObject.exe C:\\Tools\\cpu_stress_test.exe\n"
            << "  JobObject.exe C:\\Tools\\cpu_stress_test.exe 25\n";

        return EXIT_FAILURE;
    }

    const std::string programPath = argv[1];

    ResourceConfig config;

    if (argc == 3)
    {
        const int cpuPercent = std::stoi(argv[2]);

        if (cpuPercent < 1 || cpuPercent > 100)
        {
            std::cerr
                << "CPU percentage must be between 1 and 100.\n";

            return EXIT_FAILURE;
        }

        CpuConfig cpuConfig;

        cpuConfig.percent =
            static_cast<unsigned int>(cpuPercent);

        config.cpu = cpuConfig;
    }

    // Temporary affinity test:
    // 0b0011 = CPU 0 + CPU 1
    if (!config.cpu.has_value())
    {
        config.cpu = CpuConfig{};
    }

    config.cpu->affinityMask = 0b0011;

    std::cout
        << "Resource Playground - JobObject Experiment\n"
        << "--------------------------------------------\n"
        << "Program: " << programPath << '\n';

    if (config.cpu.has_value())
    {
        if (config.cpu->percent.has_value())
        {
            std::cout
                << "CPU limit: "
                << config.cpu->percent.value()
                << "%\n";
        }
        else
        {
            std::cout
                << "CPU limit: none\n";
        }

        if (config.cpu->affinityMask.has_value())
        {
            std::cout
                << "CPU affinity mask: 0x"
                << std::hex
                << config.cpu->affinityMask.value()
                << std::dec
                << '\n';
        }
    }

    std::cout << '\n';

    try
    {
        // 1. Create Job Object
        JobObject job;

        std::cout
            << "Job Object created successfully.\n";

        // 2. Apply resource configuration
        job.apply(config);

        std::cout
            << "Resource configuration applied successfully.\n";

        // 3. Create Process Launcher
        ProcessLauncher launcher;

        PROCESS_INFORMATION processInfo{};

        // 4. Create process suspended
        processInfo =
            launcher.launchSuspended(
                std::wstring(
                    programPath.begin(),
                    programPath.end()
                )
            );

        std::cout
            << "Process created.\n"
            << "PID: "
            << processInfo.dwProcessId
            << '\n';

        // 5. Assign process to Job Object
        if (!AssignProcessToJobObject(
            job.handle(),
            processInfo.hProcess))
        {
            printLastError(
                "AssignProcessToJobObject"
            );

            launcher.terminate(processInfo);
            launcher.close(processInfo);

            return EXIT_FAILURE;
        }

        std::cout
            << "Process assigned to Job Object.\n";

        // 6. Resume process
        launcher.resume(processInfo);

        std::cout
            << "Process started.\n"
            << "Waiting for process to finish...\n\n";

        // 7. Wait
        const DWORD waitResult =
            WaitForSingleObject(
                processInfo.hProcess,
                INFINITE
            );

        if (waitResult == WAIT_FAILED)
        {
            printLastError(
                "WaitForSingleObject"
            );

            launcher.close(processInfo);

            return EXIT_FAILURE;
        }

        // 8. Get exit code
        DWORD exitCode = 0;

        if (GetExitCodeProcess(
            processInfo.hProcess,
            &exitCode))
        {
            std::cout
                << "\nProcess finished.\n"
                << "Exit code: "
                << exitCode
                << '\n';
        }
        else
        {
            printLastError(
                "GetExitCodeProcess"
            );
        }

        // 9. Cleanup
        launcher.close(processInfo);
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