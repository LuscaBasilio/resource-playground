#pragma once

#include <cstdint>
#include <string>

struct MachineInfo
{
    std::wstring processorName;

    std::uint32_t physicalCores = 0;
    std::uint32_t logicalProcessors = 0;

    std::uint64_t totalMemoryBytes = 0;
};

MachineInfo getMachineInfo();