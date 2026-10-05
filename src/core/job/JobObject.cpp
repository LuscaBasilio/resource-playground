#include "JobObject.h"

#include <stdexcept>

JobObject::JobObject()
    : handle_(CreateJobObjectW(nullptr, nullptr))
{
    if (handle_ == nullptr)
    {
        throw std::runtime_error(
            "Failed to create Windows Job Object."
        );
    }
}

JobObject::~JobObject()
{
    if (handle_ != nullptr)
    {
        CloseHandle(handle_);
    }
}

HANDLE JobObject::handle() const
{
    return handle_;
}

void JobObject::apply(const ResourceConfig& config)
{
    if (config.cpu.has_value())
    {
        applyCpuConfig(*config.cpu);
    }

    if (config.memory.has_value())
    {
        applyMemoryConfig(*config.memory);
    }
}

void JobObject::applyMemoryConfig(
    const MemoryConfig& config)
{
    if (!config.limitBytes.has_value())
    {
        return;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};

    limits.BasicLimitInformation.LimitFlags =
        JOB_OBJECT_LIMIT_PROCESS_MEMORY;

    limits.ProcessMemoryLimit =
        static_cast<SIZE_T>(*config.limitBytes);

    if (!SetInformationJobObject(
        handle_,
        JobObjectExtendedLimitInformation,
        &limits,
        sizeof(limits)))
    {
        throw std::runtime_error(
            "Failed to configure memory limit."
        );
    }
}

void JobObject::applyCpuConfig(const CpuConfig& config)
{
    // CPU rate limit
    if (config.percent.has_value())
    {
        JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpuControl{};

        cpuControl.ControlFlags =
            JOB_OBJECT_CPU_RATE_CONTROL_ENABLE |
            JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;

        cpuControl.CpuRate =
            *config.percent * 100;

        if (!SetInformationJobObject(
            handle_,
            JobObjectCpuRateControlInformation,
            &cpuControl,
            sizeof(cpuControl)))
        {
            throw std::runtime_error(
                "Failed to configure CPU rate."
            );
        }
    }

    // CPU affinity
    if (config.affinityMask.has_value())
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION limits{};

        limits.BasicLimitInformation.LimitFlags =
            JOB_OBJECT_LIMIT_AFFINITY;

        limits.BasicLimitInformation.Affinity =
            static_cast<KAFFINITY>(
                *config.affinityMask
                );

        if (!SetInformationJobObject(
            handle_,
            JobObjectExtendedLimitInformation,
            &limits,
            sizeof(limits)))
        {
            throw std::runtime_error(
                "Failed to configure CPU affinity."
            );
        }
    }
}