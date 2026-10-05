#pragma once

#include "ResourceMetrics.h"

#include <windows.h>

class ResourceMonitor
{
public:
    explicit ResourceMonitor(HANDLE processHandle);

    ResourceMetrics sample();

private:
    HANDLE processHandle_;

    std::uint64_t previousProcessTime_;
    std::uint64_t previousSystemTime_;
};