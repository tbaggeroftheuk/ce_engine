#include "CE_API.hpp"

CE_EXPORT CE_PluginInfo* CE_GetPluginInfo() {
    static CE_PluginInfo info = {
        sizeof(CE_PluginInfo),
        1, 0, // plugin version
        1, 0, // engine version
        "VewyCoolPlugin",
        "Does cool stuff",
        CE_InGame | CE_UI_Main_Menu | CE_UI_InGame | CE_UI_Pause_Menu
    };
    return &info;
}

CE_EXPORT void CE_PluginInit(CE_Funcs* FuncPtr, Globals* GlobalPtr) {
    gGlobal = GlobalPtr;
    gFunc = FuncPtr;
}

CE_EXPORT void CE_PluginUpdate(void) {
    gFunc->Textures_Draw("missingtextureucantseenothing", 100, 500);
}