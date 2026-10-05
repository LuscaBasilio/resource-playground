#pragma once

#include <windows.h>

#include "C:\Users\lukal\Desktop\ResourcePlayground\src\core\resource\ResourceConfig.h"

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

    HANDLE handle_;
};