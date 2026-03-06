#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <cstdio>
#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define Rectangle WIN32_Rectangle
#define CloseWindow WIN32_CloseWindow
#define ShowCursor WIN32_ShowCursor
#include <windows.h>
#include <stdio.h>
#endif

#include "common/console_link.hpp"

static std::stringstream imguiConsoleBuffer;

class TeeBuf : public std::streambuf {
public:
    TeeBuf(std::streambuf* a, std::streambuf* b) : buf1(a), buf2(b) {}

protected:
    int overflow(int c) override {
        if (c == EOF) return !EOF;
        if (buf1) buf1->sputc(c);
        if (buf2) buf2->sputc(c);
        return c;
    }

    int sync() override {
        if (buf1) buf1->pubsync();
        if (buf2) buf2->pubsync();
        return 0;
    }

private:
    std::streambuf* buf1;
    std::streambuf* buf2;
};

static std::streambuf* originalCout = nullptr;
static TeeBuf* teeBuf = nullptr;

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

void HookImGuiConsoleOLD() {
    originalCout = std::cout.rdbuf();
    teeBuf = new TeeBuf(originalCout, imguiConsoleBuffer.rdbuf());
    std::cout.rdbuf(teeBuf);
}

const char* GetImGuiConsoleText() {
    static std::string cached;
    cached = imguiConsoleBuffer.str();
    return cached.c_str();
}

static std::streambuf* originalCerr = nullptr;

void HookImGuiConsole() {
    originalCout = std::cout.rdbuf();
    originalCerr = std::cerr.rdbuf();
    teeBuf = new TeeBuf(originalCout, imguiConsoleBuffer.rdbuf());
    std::cout.rdbuf(teeBuf);
    std::cerr.rdbuf(teeBuf);
}
