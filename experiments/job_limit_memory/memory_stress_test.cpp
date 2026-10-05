#include <windows.h>

#include <iostream>
#include <vector>

int main()
{
    std::vector<void*> allocations;

    constexpr std::size_t allocationSize =
        1024 * 1024; // 1 MB

    while (true)
    {
        void* memory =
            VirtualAlloc(
                nullptr,
                allocationSize,
                MEM_COMMIT | MEM_RESERVE,
                PAGE_READWRITE
            );

        if (memory == nullptr)
        {
            std::cerr
                << "VirtualAlloc failed.\n";

            break;
        }

        // Escreve na memória para garantir
        // que as páginas sejam realmente utilizadas.
        volatile char* bytes =
            static_cast<volatile char*>(memory);

        for (std::size_t i = 0;
            i < allocationSize;
            i += 4096)
        {
            bytes[i] = 1;
        }

        allocations.push_back(memory);

        std::cout
            << "Allocated: "
            << allocations.size()
            << " MB\n";

        Sleep(100);
    }

    for (void* memory : allocations)
    {
        VirtualFree(
            memory,
            0,
            MEM_RELEASE
        );
    }

    return 0;
}