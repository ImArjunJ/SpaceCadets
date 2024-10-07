// clang-format off
#include <windows.h>
#include <tlhelp32.h>
// clang-format on
#include <cstring>
#include <iostream>

BOOL InjectDLL(HANDLE ProcessHandle, const char* DllPath)
{
    LPVOID AllocBuffer = VirtualAllocEx(ProcessHandle, NULL, std::strlen(DllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!AllocBuffer)
    {
        std::cerr << "Failed to allocate memory in target process!\n";
        return FALSE;
    }

    if (!WriteProcessMemory(ProcessHandle, AllocBuffer, DllPath, std::strlen(DllPath) + 1, NULL))
    {
        std::cerr << "Failed to write memory in target process!\n";
        VirtualFreeEx(ProcessHandle, AllocBuffer, 0, MEM_RELEASE);
        return FALSE;
    }

    const HMODULE Kernel32 = GetModuleHandleA("kernel32.dll");
    const FARPROC LoadLibraryAddr = GetProcAddress(Kernel32, "LoadLibraryA");

    HANDLE Thread = CreateRemoteThread(ProcessHandle, NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(LoadLibraryAddr), AllocBuffer, 0, NULL);
    if (!Thread)
    {
        std::cerr << "Failed to create remote thread!\n";
        VirtualFreeEx(ProcessHandle, AllocBuffer, 0, MEM_RELEASE);
        return FALSE;
    }

    WaitForSingleObject(Thread, INFINITE);

    VirtualFreeEx(ProcessHandle, AllocBuffer, 0, MEM_RELEASE);
    CloseHandle(Thread);

    return TRUE;
}

BOOL SpawnInject(const char* NotepadPath, const char* DllPath)
{
    STARTUPINFOA StartupInfo = {0};
    PROCESS_INFORMATION ProcInfo = {0};
    StartupInfo.cb = sizeof(StartupInfo);

    if (!CreateProcessA(NotepadPath, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &StartupInfo, &ProcInfo))
    {
        std::cerr << "Failed to launch Notepad!\n";
        return FALSE;
    }

    std::cout << "Notepad launched in suspended state.\n";

    if (!InjectDLL(ProcInfo.hProcess, DllPath))
    {
        std::cerr << "Failed to inject DLL into Notepad!\n";
        TerminateProcess(ProcInfo.hProcess, 1);
        CloseHandle(ProcInfo.hProcess);
        CloseHandle(ProcInfo.hThread);
        return FALSE;
    }

    std::cout << "DLL successfully injected.\n";

    ResumeThread(ProcInfo.hThread);

    CloseHandle(ProcInfo.hProcess);
    CloseHandle(ProcInfo.hThread);

    std::cout << "Notepad resumed.\n";
    return TRUE;
}

int main()
{
    const char* NotepadPath = "C:\\Windows\\System32\\notepad.exe";
    const char* DllPath = "SCChallenge1_IAT.dll";

    if (SpawnInject(NotepadPath, DllPath))
    {
        std::cout << "Injection completed successfully!\n";
    }
    else
    {
        std::cerr << "Injection failed!\n";
    }

    return 0;
}
