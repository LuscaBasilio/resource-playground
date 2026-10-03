#include <windows.h>

#include <cstdlib>
#include <iostream>
#include <string>

namespace
{

    void printLastError(const wchar_t* operation)
    {
        const DWORD error = GetLastError();

        std::wcerr
            << operation
            << L" failed. GetLastError() = "
            << error
            << std::endl;
    }

    bool parseCpuPercent(const wchar_t* value, DWORD& cpuPercent)
    {
        wchar_t* end = nullptr;

        const unsigned long parsed = std::wcstoul(value, &end, 10);

        if (end == value || *end != L'\0')
        {
            return false;
        }

        if (parsed < 1 || parsed > 100)
        {
            return false;
        }

        cpuPercent = static_cast<DWORD>(parsed);
        return true;
    }

} // namespace

int wmain(int argc, wchar_t* argv[])
{
    if (argc < 2 || argc > 3)
    {
        std::wcerr
            << L"Usage:\n"
            << L"  job_cpu_limit.exe <program.exe> [cpu_percent]\n\n"
            << L"Example:\n"
            << L"  job_cpu_limit.exe C:\\Tools\\stress.exe 25\n";

        return EXIT_FAILURE;
    }

    const std::wstring programPath = argv[1];

    DWORD cpuPercent = 100;

    if (argc == 3)
    {
        if (!parseCpuPercent(argv[2], cpuPercent))
        {
            std::wcerr
                << L"Invalid CPU percentage. "
                << L"Expected a value between 1 and 100.\n";

            return EXIT_FAILURE;
        }
    }

    std::wcout
        << L"Resource Playground - CPU Job Experiment\n"
        << L"------------------------------------------\n"
        << L"Program:    " << programPath << L'\n'
        << L"CPU limit:  " << cpuPercent << L"%\n\n";

    // ------------------------------------------------------------
    // 1. Create Job Object
    // ------------------------------------------------------------

    HANDLE job = CreateJobObjectW(
        nullptr,
        nullptr
    );

    if (job == nullptr)
    {
        printLastError(L"CreateJobObjectW");
        return EXIT_FAILURE;
    }

    std::wcout << L"Job Object created.\n";

    // ------------------------------------------------------------
    // 2. Configure CPU rate control
    // ------------------------------------------------------------

    JOBOBJECT_CPU_RATE_CONTROL_INFORMATION cpuControl{};

    cpuControl.ControlFlags =
        JOB_OBJECT_CPU_RATE_CONTROL_ENABLE |
        JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP;

    /*
        CpuRate uses a value between 1 and 10,000.

        10,000 = 100%
        5,000  = 50%
        2,500  = 25%
        1,000  = 10%
    */

    cpuControl.CpuRate = cpuPercent * 100;

    if (!SetInformationJobObject(
        job,
        JobObjectCpuRateControlInformation,
        &cpuControl,
        sizeof(cpuControl)))
    {
        printLastError(L"SetInformationJobObject");
        CloseHandle(job);
        return EXIT_FAILURE;
    }

    std::wcout
        << L"CPU limit configured: "
        << cpuPercent
        << L"%\n";

    // ------------------------------------------------------------
    // 3. Create process
    // ------------------------------------------------------------

    /*
        CreateProcessW may modify the command-line buffer,
        therefore it must be writable.
    */

    std::wstring commandLine =
        L"\"" + programPath + L"\"";

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};

    if (!CreateProcessW(
        nullptr,
        &commandLine[0],
        nullptr,
        nullptr,
        FALSE,
        CREATE_SUSPENDED,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo))
    {
        printLastError(L"CreateProcessW");
        CloseHandle(job);
        return EXIT_FAILURE;
    }

    std::wcout
        << L"Process created.\n"
        << L"PID: "
        << processInfo.dwProcessId
        << L'\n';

    // ------------------------------------------------------------
    // 4. Assign process to Job
    // ------------------------------------------------------------

    if (!AssignProcessToJobObject(
        job,
        processInfo.hProcess))
    {
        printLastError(L"AssignProcessToJobObject");

        TerminateProcess(
            processInfo.hProcess,
            1
        );

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(job);

        return EXIT_FAILURE;
    }

    std::wcout
        << L"Process assigned to Job Object.\n";

    // ------------------------------------------------------------
    // 5. Start process
    // ------------------------------------------------------------

    if (ResumeThread(processInfo.hThread) == static_cast<DWORD>(-1))
    {
        printLastError(L"ResumeThread");

        TerminateProcess(
            processInfo.hProcess,
            1
        );

        CloseHandle(processInfo.hThread);
        CloseHandle(processInfo.hProcess);
        CloseHandle(job);

        return EXIT_FAILURE;
    }

    std::wcout
        << L"Process started.\n"
        << L"Waiting for process to finish...\n\n";

    // ------------------------------------------------------------
    // 6. Wait
    // ------------------------------------------------------------

    const DWORD waitResult = WaitForSingleObject(
        processInfo.hProcess,
        INFINITE
    );

    if (waitResult == WAIT_FAILED)
    {
        printLastError(L"WaitForSingleObject");
    }
    else
    {
        DWORD exitCode = 0;

        if (GetExitCodeProcess(
            processInfo.hProcess,
            &exitCode))
        {
            std::wcout
                << L"\nProcess finished.\n"
                << L"Exit code: "
                << exitCode
                << L'\n';
        }
        else
        {
            printLastError(L"GetExitCodeProcess");
        }
    }

    // ------------------------------------------------------------
    // 7. Cleanup
    // ------------------------------------------------------------

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    CloseHandle(job);

    return EXIT_SUCCESS;
}