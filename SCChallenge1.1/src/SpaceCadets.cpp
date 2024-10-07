#include "SpaceCadets.hpp"

#include <consoleapi.h>
#include <minwindef.h>
#include <winuser.h>

#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "IAT.hpp"

#define SPACECADETS_ID 0x1337

namespace SpaceCadets
{
    CreateWindowExWFn CreateWindowExOriginal = nullptr;
    WNDPROC WndProcOriginal = NULL;

    void Inject(HMODULE Module)
    {
        IAT::Hook("user32.dll", "CreateWindowExW", HkCreateWindowExW, reinterpret_cast<void**>(&CreateWindowExOriginal));
        IAT::Hook("user32.dll", "DefWindowProcW", HkWndProc, reinterpret_cast<void**>(&WndProcOriginal));
    }

    HWND __stdcall HkCreateWindowExW(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu,
                                     HINSTANCE hInstance, LPVOID lpParam)
    {
        const HWND OriginalResult = CreateWindowExOriginal(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
        if (wcscmp(lpClassName, L"Notepad") != 0)
            return OriginalResult;

        if (OriginalResult)
        {
            const HMENU MenuBar = GetMenu(OriginalResult);
            if (MenuBar)
            {
                const HMENU FileMenu = GetSubMenu(MenuBar, 0);
                if (FileMenu)
                {
                    AppendMenuW(FileMenu, MF_STRING, SPACECADETS_ID, L"SpaceCadets");
                }
            }
        }

        return OriginalResult;
    }

    std::string PipeCommand(const std::string& cmd)
    {
        std::array<char, 128> Buffer;
        std::string Result;

        // Open a pipe to the command
        std::unique_ptr<FILE, decltype(&_pclose)> Pipe(_popen(cmd.c_str(), "r"), _pclose);
        if (!Pipe)
        {
            throw std::runtime_error("popen() failed!");
        }

        // Read the output of the command into the buffer
        while (fgets(Buffer.data(), Buffer.size(), Pipe.get()) != nullptr)
        {
            Result += Buffer.data();
        }

        return Result;
    }

    LRESULT __stdcall HkWndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
    {
        switch (Msg)
        {
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == SPACECADETS_ID)
            {
                AllocConsole();
                FILE* StreamIn;
                FILE* StreamOut;
                FILE* StreamErr;
                freopen_s(&StreamOut, "CONOUT$", "w", stdout);
                freopen_s(&StreamIn, "CONIN$", "r", stdin);
                freopen_s(&StreamErr, "CONOUT$", "w", stderr);

                std::cout << "User Id > ";
                std::string UserId;
                std::cin >> UserId;
                std::string Command = "SCChallenge1.exe " + UserId;

                try
                {
                    const std::string Response = PipeCommand(Command);
                    HWND Edit = FindWindowExW(hWnd, NULL, L"Edit", NULL);
                    if (Edit)
                    {
                        std::wstring WideResponse(Response.begin(), Response.end());
                        SendMessageW(Edit, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(WideResponse.c_str()));
                    }
                }
                catch (const std::runtime_error& e)
                {
                    std::cerr << e.what() << std::endl;
                }

                // Refresh streams and free console
                std::cout.flush();
                std::cerr.flush();
                std::cin.clear();

                fclose(StreamOut);
                fclose(StreamIn);
                fclose(StreamErr);

                SetStdHandle(STD_OUTPUT_HANDLE, NULL);
                SetStdHandle(STD_INPUT_HANDLE, NULL);
                SetStdHandle(STD_ERROR_HANDLE, NULL);
                FreeConsole();
            }
            break;
        }
        }
        return CallWindowProc(WndProcOriginal, hWnd, Msg, wParam, lParam);
    }
} // namespace SpaceCadets