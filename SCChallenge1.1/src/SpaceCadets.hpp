#pragma once
#include <windows.h>

namespace SpaceCadets
{
    using CreateWindowExWFn = HWND(__stdcall*)(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent,
                                               HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam);
} // namespace SpaceCadets

namespace SpaceCadets
{
    void Inject(HMODULE Module);
    HWND __stdcall HkCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,

                                     HINSTANCE hInstance, LPVOID lpParam);
    LRESULT __stdcall HkWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
} // namespace SpaceCadets
