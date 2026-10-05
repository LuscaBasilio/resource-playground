#pragma once

#include <cstdint>

struct CpuMetrics
{
    double usagePercent = 0.0;
};

struct ResourceMetrics
{
    CpuMetrics cpu;
};