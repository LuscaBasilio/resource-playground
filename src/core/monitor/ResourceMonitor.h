#pragma once

#include "ResourceMetrics.h"

#include <windows.h>

#include <cstdint>

class ResourceMonitor
{
public:
    explicit ResourceMonitor(
        HANDLE processHandle,
        std::uint32_t logicalProcessorCount,
        std::uint64_t totalMemoryBytes
    );

    ResourceMetrics sample();

private:
    HANDLE processHandle_;

    std::uint32_t logicalProcessorCount_;
    std::uint64_t totalMemoryBytes_;

    std::uint64_t previousProcessTime_;
    std::uint64_t previousSystemTime_;
};