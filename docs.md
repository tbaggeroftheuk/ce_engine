# Getting started

To start using cattle engine create a folder structure like this:
```
ce_game/
├── audio/
├── Gameinfo.txt
├── scripts/
├── startup.lua
└── textures/
```

In ```Gameinfo.txt`` paste this in and change to your liking: 
```
[Gameinfo]
Game_Name: "CHANGE ME!" # Name of your game
Game_Version: "Alpha build" # This is a string, NOT A NUMBER

[Graphics]
Max_FPS: 60 # Max fps you want for your game windows
Window_Width: 1280 # Window width duh
Window_Height: 720 # Window height also duh
```

In ```startup.lua``` paste this in:
```lua
Callbacks.OnUpdate(function(dt) -- Called every frame in all states
    Text.Draw("CONGRATS, YOU'VE STARTED USING CATTLE ENGINE!", 20, 05, 100, Color(0, 0, 0))
end
)
```

# FAQ

What is ```Gameinfo.txt```?
Gameinfo.txt tells the engine some basic stuff about your game such as its name, version maximum fps etc etc.
Everything apart from Game_Name and Game_Version can be changed at runtime