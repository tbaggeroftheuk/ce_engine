#pragma once

void OpenDebugConsole();
void log2file();

void HookImGuiConsole();

void RaylibLogCallback(int logLevel, const char* text, va_list args);
const char* GetImGuiConsoleText();