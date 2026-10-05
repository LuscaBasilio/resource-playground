#include "../../src/core/machine/MachineInfo.h"
#include "../../src/core/monitor/ResourceMonitor.h"
#include "../../src/core/session/ResourceSession.h"

#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <string>

int wmain()
{
    try
    {
        // -------------------------------------------------
        // Machine information
        // -------------------------------------------------

        const MachineInfo machineInfo =
            getMachineInfo();

        std::wcout
            << L"Resource Playground - CPU Test\n"
            << L"------------------------------------------\n\n";

        std::wcout
            << L"Machine\n"
            << L"  Processor: "
            << machineInfo.processorName
            << L"\n"
            << L"  Physical cores: "
            << machineInfo.physicalCores
            << L"\n"
            << L"  Logical processors: "
            << machineInfo.logicalProcessors
            << L"\n\n";
        const double totalMemoryGB =
            static_cast<double>(machineInfo.totalMemoryBytes) /
            (1024.0 * 1024.0 * 1024.0);

        std::wcout
            << L"  Total memory: "
            << totalMemoryGB
            << L" GB\n\n";


        // -------------------------------------------------
        // Resource configuration
        // -------------------------------------------------

        ResourceConfig config;

        CpuConfig cpuConfig;

        cpuConfig.percent = 25;

        // CPU 0 + CPU 1
        cpuConfig.affinityMask = 0b0011;

        config.cpu = cpuConfig;

        MemoryConfig memoryConfig;

        memoryConfig.limitBytes =
            64ULL * 1024ULL * 1024ULL; // 64 MB

        config.memory = memoryConfig;


        std::wcout
            << L"Configuration\n"
            << L"  CPU limit: "
            << *config.cpu->percent
            << L"%\n"
            << L"  Affinity mask: 0b0011\n"
            << L"  CPUs: 0, 1\n\n";

        std::wcout
            << L"  Memory limit: "
            << (
                static_cast<double>(
                    *config.memory->limitBytes
                    ) /
                (1024.0 * 1024.0)
                )
            << L" MB\n";


        // -------------------------------------------------
        // Process
        // -------------------------------------------------

        /*const std::wstring programPath =
            L"..\\..\\experiments\\job_limit_cpu\\cpu_stress_test.exe";*/

        const std::wstring programPath =
            L"..\\..\\experiments\\job_limit_memory\\memory_stress_test.exe";

        ResourceSession session(config);

        session.start(programPath);

        std::wcout
            << L"Process\n"
            << L"  PID: "
            << session.processId()
            << L"\n"
            << L"  Status: Running\n\n";


        // -------------------------------------------------
        // Monitoring
        // -------------------------------------------------

        ResourceMonitor monitor(
            session.processHandle(),
            machineInfo.logicalProcessors,
            machineInfo.totalMemoryBytes
        );

        std::wcout
            << L"Monitoring\n"
            << L"  CPU usage:\n";


        while (session.state() ==
            ResourceSessionState::Running)
        {
            Sleep(1000);

            session.update();

            if (session.state() !=
                ResourceSessionState::Running)
            {
                break;
            }

            const ResourceMetrics metrics =
                monitor.sample();

            std::wcout
                << L"    System usage: "
                << metrics.cpu.systemUsagePercent
                << L"%\n";

            std::wcout
                << L"    Core usage:   "
                << metrics.cpu.coreUsagePercent
                << L"%\n";

            std::wcout
                << L"    Memory usage: "
                << metrics.memory.usedPercent
                << L"%\n";

            std::wcout
                << L"    Memory used:  "
                << (
                    static_cast<double>(metrics.memory.usedBytes) /
                    (1024.0 * 1024.0)
                    )
                << L" MB\n";
        }


        std::wcout
            << L"\nProcess finished.\n";
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