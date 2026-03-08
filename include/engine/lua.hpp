#include "third_party/sol/sol.hpp"

namespace CE::Lua {
    sol::state lua_state;
}
namespace CE::Lua::Functions {
    void ExposeFunctions();
}