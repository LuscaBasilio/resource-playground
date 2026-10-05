#pragma once

#include <vector>

struct CpuCoreMetrics
{
    double usagePercent = 0.0;
};

struct CpuMetrics
{
    double systemUsagePercent = 0.0;
    double coreUsagePercent = 0.0;

    std::vector<CpuCoreMetrics> cores;
};

struct MemoryMetrics
{
    std::uint64_t usedBytes = 0;
    double usedPercent = 0.0;
};

struct ResourceMetrics
{
    CpuMetrics cpu;
    MemoryMetrics memory;
};