#include <iostream>
#include <filesystem>
#include <string>

#include "globals.hpp"

#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIB(name) LoadLibraryA(name)
    #define GET_SYM(lib, name) GetProcAddress(lib, name)
    #define CLOSE_LIB(lib) FreeLibrary(lib)
    typedef HMODULE lib_handle;
#else
    #include <dlfcn.h>
    #define LOAD_LIB(name) dlopen(name, RTLD_LAZY)
    #define GET_SYM(lib, name) dlsym(lib, name)
    #define CLOSE_LIB(lib) dlclose(lib)
    typedef void* lib_handle;
#endif

void LogError(const std::string message) {
    if (CE::debug) {
        std::cout << "ERROR: CE-Modules:" << message << "\n";
        return;
    }
    return;
}
// The top and below functions are needed due to raylib colliding with windows.h
void LogInfo(const std::string message) {
    if (CE::debug) {
        std::cout << "INFO: CE-Modules:" << message << "\n";
        return;
    }
    return;
}

namespace CE::Modules {
    void Init() {

    }

    void LoadModules() {

    }

    void Update() {

    }

    void Shutdown() {
        
    }
}