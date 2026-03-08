#pragma once

#ifdef _WIN32

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define NOGDI


#include <libloaderapi.h>

#undef Rectangle
#undef CloseWindow
#undef ShowCursor
#undef LoadImage

#endif
#define LOAD_LIB(name) LoadLibraryA(name)
#define GET_SYM(lib, name) GetProcAddress(lib, name)
#define CLOSE_LIB(lib) FreeLibrary(lib)
typedef HMODULE lib_handle;