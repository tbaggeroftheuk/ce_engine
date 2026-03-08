#include "CE_API.hpp"

CE_EXPORT CE_PluginInfo* CE_GetPluginInfo() {
    static CE_PluginInfo info = {
        sizeof(CE_PluginInfo),
        1, 0, // plugin version
        1, 0, // engine version
        "VewyCoolPlugin",
        "Does cool stuff",
    };
    return &info;
}

CE_EXPORT void CE_PluginInit(CE_Funcs* FuncPtr, CE::Plugin::Globals* GlobalPtr) {
    gGlobal = GlobalPtr;
    gFunc = FuncPtr;
    gFunc->Log(LOG_INFO, "The plugin has got the globals!");
}

bool shownLog = false;

CE_EXPORT void CE_PluginUpdate(void) {
    gFunc->TexturesDraw("missingtextureucantseenothing", 100, 500);
    if(!shownLog) {
        gFunc->Log(LOG_INFO, "The plugin has updated!");
        shownLog = true;
    }
}

CE_EXPORT void CE_PluginShutdown(void) {
    gFunc = nullptr;
    gGlobal = nullptr;
}