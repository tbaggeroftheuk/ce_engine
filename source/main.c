#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#include <raylib.h>

#if defined(_WIN32) || defined(_WIN64)
#define NOGDI           
#define NOUSER     
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#undef NOUSER      
#endif

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "bootstrap.h"
#include "globals.h"
#include "engine/engine.h"

#ifdef _WIN32 // Yes it's a hack to get it run on windows
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
    (void)hInstance;
    (void)hPrevInstance;
    (void)nShowCmd;

    const char *tmp = getenv("TMP");
    if (!tmp) tmp = getenv("TEMP");
    if (!tmp) tmp = ".";

    strncpy(ce_globals.path, tmp, sizeof(ce_globals.path)-1);
    ce_globals.base_path[sizeof(ce_globals.path)-1] = '\0';

    char *argv[] = { "hello.exe", lpCmdLine };
    int argc = (lpCmdLine && lpCmdLine[0]) ? 2 : 1;
    return main(argc, argv);
}
#endif


int main(int argc, char *argv[]) {
    SetRandomSeed(time(NULL));
    if (argc > 1 && strcmp(argv[1], "debug-mode") == 0) {
        ce_globals.debug = true;
    }
    if (ce_globals.debug) {
        SetTraceLogLevel(LOG_ALL);
        ce_bootstrap();
        ce_exit_debug();
    } else {
        SetTraceLogLevel(LOG_NONE);
        ce_bootstrap();
        ce_exit();
    }
    return 0;
}