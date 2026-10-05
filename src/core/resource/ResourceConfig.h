#pragma once

#include <optional>
#include <cstdint>

struct CpuConfig
{
    std::optional<unsigned int> percent;
    std::optional<std::uint64_t> affinityMask;
};

struct MemoryConfig
{
    std::optional<std::uint64_t> limitBytes;
};

class ResourceConfig
{
public:
    std::optional<CpuConfig> cpu;
    std::optional<MemoryConfig> memory;
};