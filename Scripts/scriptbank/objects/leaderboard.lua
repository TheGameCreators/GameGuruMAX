-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Leaderboard v3 by Necrym59
-- DESCRIPTION: Will create a leaderboard of highest scores using a designated UserGlobal.
-- DESCRIPTION: Attach to an object set AlwaysActive=On.
-- DESCRIPTION: [USER_GLOBAL$=""] User Global used for current game score (eg; MyPoints)
-- DESCRIPTION: [HISCORE1_GLOBAL$=""] User Global used to display High Score 1
-- DESCRIPTION: [HISCORE2_GLOBAL$=""] User Global used to display High Score 2
-- DESCRIPTION: [HISCORE3_GLOBAL$=""] User Global used to display High Score 3
-- DESCRIPTION: [HISCORE4_GLOBAL$=""] User Global used to display High Score 4
-- DESCRIPTION: [HISCORE5_GLOBAL$=""] User Global used to display High Score 5
-- DESCRIPTION: [LEADERBOARD_HUD$=""] eg: "HUD Screen 3"
-- DESCRIPTION: [DISPLAY_AT_START!=0] will display hud at startup.

local leaderboard 		= {}
local user_global		= {}
local hiscore1_global 	= {}
local hiscore2_global 	= {}
local hiscore3_global 	= {}
local hiscore4_global 	= {}
local hiscore5_global 	= {}
local leaderboard_hud	= {}
local display_at_start	= {}

local highscore1		= {}
local highscore2		= {}
local highscore3		= {}
local highscore4		= {}
local highscore5		= {}
local currentvalue		= {}
local startdisplay		= {}
local status 			= {}

function leaderboard_properties(e, user_global, hiscore1_global, hiscore2_global, hiscore3_global, hiscore4_global, hiscore5_global, leaderboard_hud, display_at_start)
	leaderboard[e].user_global = user_global
	leaderboard[e].hiscore1_global = hiscore1_global
	leaderboard[e].hiscore2_global = hiscore2_global
	leaderboard[e].hiscore3_global = hiscore3_global
	leaderboard[e].hiscore4_global = hiscore4_global
	leaderboard[e].hiscore5_global = hiscore5_global
	leaderboard[e].leaderboard_hud = leaderboard_hud
	leaderboard[e].display_at_start = display_at_start
end 

function leaderboard_init(e)
	leaderboard[e] = {}
	leaderboard[e].user_global = ""	
	leaderboard[e].hiscore1_global = ""
	leaderboard[e].hiscore2_global = ""
	leaderboard[e].hiscore3_global = ""
	leaderboard[e].hiscore4_global = ""
	leaderboard[e].hiscore5_global = ""
	leaderboard[e].leaderboard_hud = ""
	leaderboard[e].display_at_start = 0
	
	status[e] = "init"	
	currentvalue[e] = 0
	startdisplay[e] = 0
	highscore1[e] = 0
	highscore2[e] = 0
	highscore3[e] = 0
	highscore4[e] = 0
	highscore5[e] = 0
end

function leaderboard_main(e)
	if status[e] == "init" then
		status[e] = "loadscores"
	end
	
	if status[e] == "loadscores" then 
		local file = io.open("databank\\leaderboard.dat", "r")
		if (file) then
			highscore1[e] = file:read("*n", "*l")
			highscore2[e] = file:read("*n", "*l")
			highscore3[e] = file:read("*n", "*l")
			highscore4[e] = file:read("*n", "*l")
			highscore5[e] = file:read("*n", "*l")			
			file:close() -- Close the file if it did successfully open.
			_G["g_UserGlobal['"..leaderboard[e].hiscore1_global.."']"] = highscore1[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore2_global.."']"] = highscore2[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore3_global.."']"] = highscore3[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore4_global.."']"] = highscore4[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore5_global.."']"] = highscore5[e]
		else
			local file = io.open("databank\\leaderboard.dat", "w")
			file:write(highscore1[e].."\n")
			file:write(highscore2[e].."\n")
			file:write(highscore3[e].."\n")
			file:write(highscore4[e].."\n")
			file:write(highscore5[e].."\n")			
			file:close() -- Close the file if new file created.
		end
		status[e] = "waiting"
	end
	
	if status[e] == "waiting" then
		if leaderboard[e].user_global ~= "" then
			if _G["g_UserGlobal['"..leaderboard[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..leaderboard[e].user_global.."']"] end
			if currentvalue[e] > highscore1[e] then				
				highscore5[e] = highscore4[e]
				highscore4[e] = highscore3[e]				
				highscore3[e] = highscore2[e]
				highscore2[e] = highscore1[e]
				highscore1[e] = currentvalue[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore1_global.."']"] = highscore1[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore2_global.."']"] = highscore2[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore3_global.."']"] = highscore3[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore4_global.."']"] = highscore4[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore5_global.."']"] = highscore5[e]
				status[e] = "savescores"
			end
		end
		if leaderboard[e].display_at_start == 1 and startdisplay[e] == 0 then			
			if leaderboard[e].leaderboard_hud > "" then
				ScreenToggle(leaderboard[e].leaderboard_hud)
				startdisplay[e] = 1
			end			
		end
	end
	
	if status[e] == "savescores" then
		local file = io.open("databank\\leaderboard.dat", "w")
		file:write(highscore1[e].."\n")
		file:write(highscore2[e].."\n")
		file:write(highscore3[e].."\n")
		file:write(highscore4[e].."\n")
		file:write(highscore5[e].."\n")		
		file:close()
		status[e] = "loadscores"
	end
end	