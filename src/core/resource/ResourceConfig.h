#pragma once

#include <optional>
#include <cstdint>

struct CpuConfig
{
    std::optional<unsigned int> percent;
    std::optional<std::uint64_t> affinityMask;
};

class ResourceConfig
{
public:
    std::optional<CpuConfig> cpu;
};