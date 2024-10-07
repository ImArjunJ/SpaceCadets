#include <windows.h>

#include "SpaceCadets.hpp"


int __stdcall DllMain(HMODULE Module, DWORD Reason, LPVOID Reserved)
{
    switch (Reason)
    {
    case DLL_PROCESS_ATTACH:
        SpaceCadets::Inject(Module);
        break;
    }
    return 1;
}
