#include "../../src/core/machine/MachineInfo.h"

#include <cstdlib>
#include <iostream>

int wmain()
{
    try
    {
        const MachineInfo info =
            getMachineInfo();

        std::wcout
            << L"Resource Playground - Machine Information\n"
            << L"------------------------------------------\n\n";

        std::wcout
            << L"Processor:\n"
            << L"  "
            << info.processorName
            << L"\n\n";

        std::wcout
            << L"Physical cores:\n"
            << L"  "
            << info.physicalCores
            << L"\n\n";

        std::wcout
            << L"Logical processors:\n"
            << L"  "
            << info.logicalProcessors
            << L"\n";
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