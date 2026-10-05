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

struct ResourceMetrics
{
    CpuMetrics cpu;
};