# all of dis is a C abi
## ```CEPluginInfo* CE_PluginInfo()```
Description: Let the engine get plugin infomation
This should return a struct
```c
// Inside a plugin
#include <string.h>

static CE_PluginInfo info = {
    .StructSize = sizeof(CE_PluginInfo),
    // Plugin ver
    .Plugin_Version_Major = 1,
    .Plugin_Version_Minor = 0,
    // Engine ver
    .CE_Engine_Version_Major = 1,
    .CE_Engine_version_Minor = 0,
    // Plugin infomation
    .Name = "VewyCoolPlugin",
    .Description = "Does cool shit",

    // Plugin capabilities
    .Lua_Injects = 0, // 0 Means false
    .InGame = 1,
    .UI_Main_Menu = 1,
    .UI_InGame = 1,
    .UI_Pause_Menu = 1,
};


extern "C" CE_PluginInfo* CE_GetPluginInfo() {
    return &info;
}
```

## ```void CE_PluginInit(CE_Funcs* FuncPtr, Globals* GlobalPtr)```
Description: Get pointers to engine functions and init basic shite
```c++
Globals* gGlobal = nullptr;
CE_Funcs gFuncs = nullptr;

// You can add other stuff but copying the pointers to the globals is to get engine state and use engine functions 
extern "C" void CE_PluginInit(CE_Funcs* FuncPtr, Globals* GlobalPtr) {
    gGlobal = GlobalPtr;
    gFuncs = FuncPtr;
}
```


## ```void CE_PluginUpdateUI_InGame()```
