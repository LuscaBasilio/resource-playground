#pragma once

#include <optional>

struct CpuConfig
{
    unsigned int percent;
};

class ResourceConfig
{
public:
    std::optional<CpuConfig> cpu;
};