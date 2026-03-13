#include "engine/assets/assets.hpp"
#include "engine/plugins/plugins.hpp"
#include "engine/lua.hpp"

extern "C" {
    #include <raylib.h>
}


namespace CE {
    void Shutdown() {
        CE::Assets::Textures::Shutdown();
        CE::Assets::Audio::Shutdown();
        CE::Assets::Fonts::UnloadAll();
        CE::Plugins::Shutdown();
        CE::Lua::Shutdown();
        CloseWindow();
    }
}