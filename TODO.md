# Make game state more modular

You call 
```lua 
CE.RegisterGameState("MainMenu")
```

this does 

```c++
void reg_gamestate(const char* gamestate) {
    char* plugin_symbol_name = "CE_PluginUpdate_" + gamestate;
    ce_plugin_state symbol = GET_PLUGIN_SYMBOL(plugin_symbol_name);
    store_plugin_state(symbol);
}
```