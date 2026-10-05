#pragma once

#include <windows.h>

#include "../resource/ResourceConfig.h"

class JobObject
{
public:
    JobObject();
    ~JobObject();

    JobObject(const JobObject&) = delete;
    JobObject& operator=(const JobObject&) = delete;

    void apply(const ResourceConfig& config);

    HANDLE handle() const;

private:
    void applyCpuConfig(const CpuConfig& config);
    void applyMemoryConfig(const MemoryConfig& config);

    HANDLE handle_;
};