#pragma once

namespace SpaceCadets::IAT
{
    void Hook(const char* Module, const char* Function, void* HookFn, void** OriginalFn);
}