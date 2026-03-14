#pragma once

#include <cstddef>
#include <iostream>
#include <fstream>
#include <cstdint>
#if defined(_WIN32)

#define NOMINMAX            // avoids min/max macros
#define WIN32_LEAN_AND_MEAN // exclude rarely-used stuff
#define NOGDI               // exclude GDI (includes Rectangle)
#define NOCOMM              // exclude serial communication
#define NOKERNEL            // optional, only if you don't use kernel APIs
#define NOUSER              // excludes user APIs like CloseWindow, ShowCursor
    #define _WIN32_WINNT 0x0602 // Windows 8
    #include <windows.h>
    #include <psapi.h>
    #define NOMINMAX
#elif defined(__linux__) || defined(__APPLE__)
    #include <sys/resource.h>
    #include <pthread.h>
    #include <unistd.h>
    #if defined(__APPLE__)
        #include <mach/mach.h>
    #endif
#else
    #error "Unsupported platform"
#endif

namespace MemoryUsage {

    inline size_t getStackLimit() {
    #if defined(_WIN32)
        ULONG_PTR low, high;
        GetCurrentThreadStackLimits(&low, &high);
        return high - low;
    #elif defined(__linux__) || defined(__APPLE__)
        struct rlimit rl;
        if (getrlimit(RLIMIT_STACK, &rl) == 0)
            return rl.rlim_cur;
        return 0;
    #endif
    }

    inline size_t getHeapUsage() {
#if defined(_WIN32)
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(),
                             reinterpret_cast<PROCESS_MEMORY_COUNTERS*>(&pmc),
                             sizeof(pmc))) {
        return pmc.PrivateUsage;
    }
#elif defined(__linux__)
    std::ifstream file("/proc/self/status");
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("VmData:") == 0) { 
            size_t kb;
            sscanf(line.c_str(), "VmData: %zu kB", &kb);
            return kb * 1024;
        }
    }
    return 0;
#elif defined(__APPLE__)
    mach_task_basic_info info;
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                  (task_info_t)&info, &count) == KERN_SUCCESS) {
        return info.resident_size;
    }
    return 0;
#endif

    // Default return if none of the above platforms match
    return 0;
}
    inline size_t getResidentMemory() {
    #if defined(_WIN32)
        PROCESS_MEMORY_COUNTERS pmc;
        if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
            return pmc.WorkingSetSize;
        }
        return 0;
    #elif defined(__linux__)
        std::ifstream file("/proc/self/status");
        std::string line;
        while (std::getline(file, line)) {
            if (line.find("VmRSS:") == 0) { 
                size_t kb;
                sscanf(line.c_str(), "VmRSS: %zu kB", &kb);
                return kb * 1024;
            }
        }
        return 0;
    #elif defined(__APPLE__)
        mach_task_basic_info info;
        mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
        if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
                      (task_info_t)&info, &count) == KERN_SUCCESS) {
            return info.resident_size;
        }
        return 0;
    #endif
    }

    inline void printMemoryUsage() {
        std::cout << "Stack Limit: " << getStackLimit() / 1024 << " KB\n";
        std::cout << "Heap / Private Memory: " << getHeapUsage() / 1024 << " KB\n";
        std::cout << "Resident Memory (RAM): " << getResidentMemory() / 1024 << " KB\n";
    }

}