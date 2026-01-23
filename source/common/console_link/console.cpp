#include <iostream>

#if defined(_WIN32)
    #define WIN32_LEAN_AND_MEAN
    #define Rectangle WIN32_Rectangle
    #define CloseWindow WIN32_CloseWindow
    #define ShowCursor WIN32_ShowCursor
    #include <windows.h>
    #include <stdio.h>
#endif

#include "common/console_link.hpp"

void OpenDebugConsole() {
#if defined(_WIN32)
    AllocConsole();

    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    std::cout << "INFO: CE-DEBUG: Debug Mode On\n";
#endif
}