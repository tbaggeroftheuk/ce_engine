#include <iostream>
#include <string>

#include "common/console_link.hpp"

#include "engine/common.hpp"
#include "globals.hpp"
#include "bootstrap.hpp"

int main(int argc, char *argv[]) {

    // Check for flags
    for (int I = 1; I < argc; I++) {
        if (std::string(argv[I]) == "--debug") { // Enable global logging
            CE::Debug = true;
        }

        if(std::string(argv[I]) == "--plugin-logs") { // Show plugin logs
            CE::Flags::show_plugin_logs = true;
        }

        if(std::string(argv[I]) == "--bypass-crc") { // Bypass crc check of the data file
            CE::Flags::bypass_data_file_crc_crash = true;
        }

        if(std::string(argv[I]) == "--log2file") { // Log to a file instead of terminal
            log2file();
        }

        if(std::string(argv[I]) == "--licences") { // Display licences to be legals
            OpenDebugConsole();
            printLicences();
            std::cout << "Press enter to continue...";
            std::cin.get(); // wait for for enter to be pressed
            return 0;
        }
    }

    if (CE::Debug) {
        OpenDebugConsole();
        SetTraceLogLevel(LOG_ALL);
        TraceLog(LOG_INFO, "CE: Debug is currently activated");
    } else {
        TraceLog(LOG_INFO, "CE Debug is off");
        SetTraceLogLevel(LOG_NONE);
    }

    TraceLog(LOG_INFO, "CE: Cattle engine: No BULLSHIT");
    TraceLog(LOG_INFO, "CE: Engine version: %s", CE::engine_ver.c_str());

    CE::Bootstrap(); // Actually setting up the files, window and other stuff

    return 0;
}
