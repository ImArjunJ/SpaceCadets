#include "IAT.hpp"

#include <windows.h>

#include <cstdint>

namespace SpaceCadets::IAT
{
    void Hook(const char* Module, const char* Function, void* HookFn, void** OriginalFn)
    {
        const HMODULE ProcessHandle = GetModuleHandleA(NULL);
        if (!ProcessHandle)
        {
            MessageBoxA(nullptr, "Failed to get module handle", "SpaceCadets", MB_ICONERROR);
            return;
        }

        const PIMAGE_DOS_HEADER DosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(ProcessHandle);

        const PIMAGE_NT_HEADERS NtHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<std::uintptr_t>(ProcessHandle) + DosHeader->e_lfanew);

        PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = reinterpret_cast<PIMAGE_IMPORT_DESCRIPTOR>(
            reinterpret_cast<std::uintptr_t>(ProcessHandle) + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

        while (ImportDescriptor->Name)
        {
            const char* ModuleName = reinterpret_cast<const char*>(reinterpret_cast<std::uintptr_t>(ProcessHandle) + ImportDescriptor->Name);

            if (_stricmp(ModuleName, Module) == 0)
            {
                PIMAGE_THUNK_DATA OriginalThunk = reinterpret_cast<PIMAGE_THUNK_DATA>(reinterpret_cast<std::uintptr_t>(ProcessHandle) + ImportDescriptor->OriginalFirstThunk);
                PIMAGE_THUNK_DATA Thunk = reinterpret_cast<PIMAGE_THUNK_DATA>(reinterpret_cast<std::uintptr_t>(ProcessHandle) + ImportDescriptor->FirstThunk);

                while (OriginalThunk->u1.AddressOfData)
                {
                    const PIMAGE_IMPORT_BY_NAME ImportByName =
                        reinterpret_cast<PIMAGE_IMPORT_BY_NAME>(reinterpret_cast<std::uintptr_t>(ProcessHandle) + OriginalThunk->u1.AddressOfData);

                    if (_stricmp(reinterpret_cast<const char*>(ImportByName->Name), Function) == 0)
                    {
                        DWORD OldProtect;
                        VirtualProtect(&Thunk->u1.Function, sizeof(Thunk->u1.Function), PAGE_READWRITE, &OldProtect);

                        *OriginalFn = reinterpret_cast<void*>(Thunk->u1.Function);
                        Thunk->u1.Function = reinterpret_cast<std::uintptr_t>(HookFn);

                        VirtualProtect(&Thunk->u1.Function, sizeof(Thunk->u1.Function), OldProtect, &OldProtect);

                        return;
                    }

                    ++OriginalThunk;
                    ++Thunk;
                }
            }

            ++ImportDescriptor;
        }
    }
} // namespace SpaceCadets::IAT