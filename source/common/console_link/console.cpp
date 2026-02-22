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
#else
    return;
#endif
}

#include <iostream>
#include <fstream>

void log2file() {
#if defined(_WIN32)
    static std::ofstream logfile("CE-Debug.log", std::ios::out | std::ios::trunc);

    if (logfile.is_open()) {
        logfile << "INFO: CE-DEBUG: Debug Mode On\n";
    }
#else
    freopen("CE-Debug.log", "w", stdout);
    freopen("CE-Debug.log", "w", stderr);
    std::cout << "INFO: CE-DEBUG: Debug Mode On\n";
#endif
}