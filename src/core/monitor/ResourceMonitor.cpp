#include "ResourceMonitor.h"

#include <stdexcept>

namespace
{

    std::uint64_t fileTimeToUInt64(
        const FILETIME& fileTime
    )
    {
        ULARGE_INTEGER value{};

        value.LowPart = fileTime.dwLowDateTime;
        value.HighPart = fileTime.dwHighDateTime;

        return value.QuadPart;
    }

} // namespace

ResourceMonitor::ResourceMonitor(
    HANDLE processHandle
)
    : processHandle_(processHandle),
    previousProcessTime_(0),
    previousSystemTime_(0)
{
    if (processHandle_ == nullptr)
    {
        throw std::invalid_argument(
            "Process handle cannot be null."
        );
    }
}

ResourceMetrics ResourceMonitor::sample()
{
    FILETIME creationTime{};
    FILETIME exitTime{};
    FILETIME kernelTime{};
    FILETIME userTime{};

    if (!GetProcessTimes(
        processHandle_,
        &creationTime,
        &exitTime,
        &kernelTime,
        &userTime))
    {
        throw std::runtime_error(
            "Failed to get process times."
        );
    }

    FILETIME idleTime{};
    FILETIME kernelSystemTime{};
    FILETIME userSystemTime{};

    if (!GetSystemTimes(
        &idleTime,
        &kernelSystemTime,
        &userSystemTime))
    {
        throw std::runtime_error(
            "Failed to get system times."
        );
    }

    const std::uint64_t processTime =
        fileTimeToUInt64(kernelTime) +
        fileTimeToUInt64(userTime);

    const std::uint64_t systemTime =
        fileTimeToUInt64(kernelSystemTime) +
        fileTimeToUInt64(userSystemTime);

    ResourceMetrics metrics{};

    if (previousProcessTime_ != 0 &&
        previousSystemTime_ != 0)
    {
        const std::uint64_t processDelta =
            processTime - previousProcessTime_;

        const std::uint64_t systemDelta =
            systemTime - previousSystemTime_;

        if (systemDelta > 0)
        {
            metrics.cpu.usagePercent =
                (static_cast<double>(processDelta) /
                    static_cast<double>(systemDelta)) *
                100.0;
        }
    }

    previousProcessTime_ = processTime;
    previousSystemTime_ = systemTime;

    return metrics;
}