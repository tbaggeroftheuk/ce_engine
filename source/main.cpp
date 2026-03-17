#include <iostream>
#include <string>

#include "common/console_link.hpp"

#include "engine/common.hpp"
#include "globals.hpp"
#include "bootstrap.hpp"

extern "C" {
    #include <raylib.h>
}


#define ANSI_RESET   "\033[0m"
#define ANSI_RED     "\033[31m"
#define ANSI_YELLOW  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_CYAN    "\033[36m"

void RaylibLogCallback(int logLevel, const char* text, va_list args) {
    char buffer[2048];
    vsnprintf(buffer, sizeof(buffer), text, args);

    const char* levelStr = "";
    switch (logLevel) {
        case LOG_TRACE:   levelStr = "TRACE"; break;
        case LOG_DEBUG:   levelStr = "DEBUG"; break;
        case LOG_INFO:    levelStr = "INFO"; break;
        case LOG_WARNING: levelStr = "WARNING"; break;
        case LOG_ERROR:   levelStr = "ERROR"; break;
        case LOG_FATAL:   levelStr = "FATAL"; break;
        default:          levelStr = "UNKNOWN"; break;
    }

    std::cout << levelStr << ": " << buffer << std::endl;
}

int main(int argc, char *argv[]) {

    // Check for flags
    for (int I = 1; I < argc; I++) {
    std::string arg = argv[I];

    if (arg == "--debug") { // Enable global logging
        CE::Debug = true;
    }

    if (arg == "--plugin-logs") { // Show plugin logs
        CE::Flags::show_plugin_logs = true;
    }

    if (arg == "--bypass-crc") { // Bypass crc check of the data file
        CE::Flags::bypass_data_file_crc_crash = true;
    }

    if (arg == "--log2file") { // Log to a file instead of terminal
        log2file();
    }

    if (arg == "--licences") { // Display licences to be legals
        OpenDebugConsole();
        printLicences();
        std::cout << "Press enter to continue...";
        std::cin.get(); // wait for enter
        return 0;
    }

    // Handle custom data file
    const std::string prefix = "--datafile=";
    if (arg.rfind(prefix, 0) == 0) { // starts with "--datafile="
        CE::DATA_FILE_NAME = arg.substr(prefix.size()); // everything after '='
        TraceLog(LOG_INFO, "CE: Custom data file set: %s", CE::DATA_FILE_NAME.c_str());
    }

    }

    if (CE::Debug) {
        OpenDebugConsole();
        SetTraceLogCallback(RaylibLogCallback);
        HookImGuiConsole();
        SetTraceLogLevel(LOG_ALL);
        TraceLog(LOG_INFO, "CE: Debug is currently activated");
    } else {
        TraceLog(LOG_INFO, "CE Debug is off");
        SetTraceLogLevel(LOG_NONE);
    }

    TraceLog(LOG_INFO, "CE: Cattle engine: No BULLSHIT");
    TraceLog(LOG_INFO, "CE: Engine version: %s", CE::engine_ver.c_str());

    CE::Bootstrap(); // Actually setting up the files, window and other stuff

    CE::Shutdown(); // Unload EVERYTHING

    return 0;
}
