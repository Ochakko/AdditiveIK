// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"

HINSTANCE g_hInstance = 0;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{

    g_hInstance = (HINSTANCE)hModule;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
