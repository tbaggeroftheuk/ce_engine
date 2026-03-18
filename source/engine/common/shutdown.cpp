#include "engine/assets/assets.hpp"
#include "engine/plugins/plugins.hpp"
#include "engine/lua.hpp"
#include "engine/callbacks.hpp"
#include "globals.hpp"

extern "C" {
    #include <raylib.h>
}

namespace CE {
    void Shutdown(int returnVal) {
        CE::Assets::Textures::Shutdown();
        CE::Assets::Audio::Shutdown();
        CE::Assets::Fonts::UnloadAll();
        CE::Plugins::Shutdown();
        CE::Lua::Shutdown();
        if(!CE::isWinowOpen) {
            CloseWindow();
        }
        std::exit(returnVal);
    }
}
