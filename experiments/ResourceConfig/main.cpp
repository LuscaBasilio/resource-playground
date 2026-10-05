#include "../../src/core/resource/ResourceConfig.cpp"

#include <iostream>

int main()
{
    // ------------------------------------------------------------
    // ResourceConfig sem nenhuma limitação
    // ------------------------------------------------------------

    ResourceConfig unrestricted;

    std::cout << "Unrestricted configuration:\n";

    if (!unrestricted.cpu.has_value())
    {
        std::cout << "CPU: no limit\n";
    }

    // ------------------------------------------------------------
    // ResourceConfig com limite de CPU
    // ------------------------------------------------------------

    ResourceConfig cpuLimited;

    CpuConfig cpuConfig;
    cpuConfig.percent = 25;

    cpuLimited.cpu = cpuConfig;

    std::cout << "\nCPU limited configuration:\n";

    if (cpuLimited.cpu.has_value())
    {
        std::cout
            << "CPU: "
            << cpuLimited.cpu->percent.value()
            << "%\n";
    }

    return 0;
}