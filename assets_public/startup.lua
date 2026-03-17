CE.SetGameState("Main_Menu")

Log.Info("This is an info message")
Log.Debug("This is a debug message")
Log.Warn("This is a warning message")
Log.Error("This is an error message")

Audio.Music.Load("music.mp3", "TestMusic")


local playerPosX = 0
local playerPosY = 0
Textures.Load("player", "textures/player.jpg")


local function PlayerUpdate()
    if Input.IsKeyDown("LEFT") then
        playerPosX = playerPosX + -5
    end

    if Input.IsKeyDown("RIGHT") then
        playerPosX = playerPosX + 5
    end

    if Input.IsKeyDown("UP") then
        playerPosY = playerPosY + -5
    end

    if Input.IsKeyDown("DOWN") then
        playerPosY = playerPosY + 5
    end

    Textures.Draw("player", playerPosX, playerPosY)
end

function Update()
    Textures.Draw("LOL_FROM_LUA_NO_EXISTING_TEXTURE", 100, 200, Color(255, 100, 50, 200))
    
    Text.Draw("Press A to set game state to change gamestate to InGame,", 20, 05, 100, Color(0, 0, 0))
    Text.Draw("S to set it to Main_Menu", 20, 5, 120, Color(0, 0, 0))

    if Input.IsKeyPressed("A") then
        CE.SetGameState("InGame")
    end

    if Input.IsKeyPressed("S") then
        CE.SetGameState("Main_Menu")
    end

    Text.Draw("Current state: ", 20, 5, 160, Color(0, 0, 0))
end

Callbacks.On("InGame", "Draw", function(state, event)
  Text.Draw("Ingame", 20, 165, 160, Color(0,0,0,255))
  Audio.Music.Pause("TestMusic")
  PlayerUpdate()
end)

Callbacks.On("Main_Menu", "Draw", function(state, event)
  Text.Draw("Main_Menu", 20, 165, 160, Color(0,0,0,255))
end)