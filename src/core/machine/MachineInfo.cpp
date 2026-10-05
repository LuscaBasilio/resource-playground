#include "MachineInfo.h"

#include <windows.h>

#include <stdexcept>
#include <vector>

namespace
{

    std::wstring getProcessorName()
    {
        HKEY key = nullptr;

        const LONG result =
            RegOpenKeyExW(
                HKEY_LOCAL_MACHINE,
                L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
                0,
                KEY_READ,
                &key
            );

        if (result != ERROR_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to open processor registry key."
            );
        }

        wchar_t buffer[256]{};
        DWORD bufferSize = sizeof(buffer);
        DWORD type = 0;

        const LONG queryResult =
            RegQueryValueExW(
                key,
                L"ProcessorNameString",
                nullptr,
                &type,
                reinterpret_cast<LPBYTE>(buffer),
                &bufferSize
            );

        RegCloseKey(key);

        if (queryResult != ERROR_SUCCESS)
        {
            throw std::runtime_error(
                "Failed to read processor name."
            );
        }

        return std::wstring(buffer);
    }

    void getProcessorTopology(
        std::uint32_t& physicalCores,
        std::uint32_t& logicalProcessors
    )
    {
        DWORD bufferSize = 0;

        if (GetLogicalProcessorInformationEx(
            RelationProcessorCore,
            nullptr,
            &bufferSize))
        {
            throw std::runtime_error(
                "Unexpected processor information result."
            );
        }

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
        {
            throw std::runtime_error(
                "Failed to query processor information size."
            );
        }

        std::vector<BYTE> buffer(bufferSize);

        auto* information =
            reinterpret_cast<PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX>(
                buffer.data()
                );

        if (!GetLogicalProcessorInformationEx(
            RelationProcessorCore,
            information,
            &bufferSize))
        {
            throw std::runtime_error(
                "Failed to query processor information."
            );
        }

        DWORD offset = 0;

        while (offset < bufferSize)
        {
            auto* entry =
                reinterpret_cast<
                PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX
                >(
                    buffer.data() + offset
                    );

            if (entry->Relationship == RelationProcessorCore)
            {
                ++physicalCores;

                for (WORD group = 0;
                    group < entry->Processor.GroupCount;
                    ++group)
                {
                    KAFFINITY mask =
                        entry->Processor.GroupMask[group].Mask;

                    while (mask != 0)
                    {
                        ++logicalProcessors;

                        mask &= (mask - 1);
                    }
                }
            }

            offset += entry->Size;
        }
    }

    std::uint64_t getTotalMemory()
    {
        MEMORYSTATUSEX memoryStatus{};

        memoryStatus.dwLength =
            sizeof(memoryStatus);

        if (!GlobalMemoryStatusEx(&memoryStatus))
        {
            throw std::runtime_error(
                "Failed to query system memory."
            );
        }

        return memoryStatus.ullTotalPhys;
    }

} // namespace

MachineInfo getMachineInfo()
{
    MachineInfo info;

    info.processorName =
        getProcessorName();

    getProcessorTopology(
        info.physicalCores,
        info.logicalProcessors
    );

    info.totalMemoryBytes =
        getTotalMemory();

    return info;
}