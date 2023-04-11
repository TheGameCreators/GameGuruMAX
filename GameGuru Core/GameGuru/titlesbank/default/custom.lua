-- custom page script

g_strStyleFolder = ""
g_imgCursor = 0
g_sprCursor = 0
g_sprCursorPtrX = 50
g_sprCursorPtrY = 33
g_sprCursorPtrClick = 0
g_activecustomscreen = ""

function custom_init(customscreen)
 if customscreen  ~= nil then
  g_activecustomscreen = customscreen .. ".lua"
 else
  g_activecustomscreen = "custom1.lua"
 end
 -- determine style folder we are in
 file = io.open("titlesbank\\style.txt", "r")
 if file ~= nil then
  io.input(file)
  g_strStyleFolder = io.read()
  io.close(file)
 end
 -- read graphics page settings
 file = io.open("savegames\\graphics.dat", "r")
 if file ~= nil then
  io.input(file)
  iGraphicChoice = tonumber(io.read())
  SetGameQuality(iGraphicChoice)
  io.close(file)
 end
 -- read graphics page settings
 file = io.open("savegames\\sounds.dat", "r")
 if file ~= nil then
  io.input(file)
  iSoundChoice = tonumber(io.read())
  iMusicChoice = tonumber(io.read())
  SetGameSoundVolume ( iSoundChoice )
  SetGameMusicVolume ( iMusicChoice )
  io.close(file)
 end
   -- cursor
 g_imgCursor = LoadImage("titlesbank\\" .. g_strStyleFolder .. "\\cursor.png")
 g_sprCursor = CreateSprite ( g_imgCursor )
 SetSpritePriority(g_sprCursor, -1) 
 ActivateMouse()
end

function custom_main()
 -- control menus
 cursorControl = require "titlesbank\\cursorcontrol"
 g_sprCursorPtrX,g_sprCursorPtrY,g_sprCursorPtrClick = cursorControl.getinput(g_sprCursorPtrX,g_sprCursorPtrY,g_sprCursorPtrClick)
 SetSpritePosition ( g_sprCursor, g_sprCursorPtrX, g_sprCursorPtrY )

 DisplayScreen(g_activecustomscreen)
 
end

function custom_free()
 
 DeleteSprite ( g_sprCursor )
 DeleteImage ( g_imgCursor )
 DeactivateMouse()
end
