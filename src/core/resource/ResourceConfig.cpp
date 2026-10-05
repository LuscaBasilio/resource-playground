#pragma once

#include <optional>

struct CpuConfig
{
    unsigned int percent;
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