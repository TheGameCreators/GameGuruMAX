-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Leaderboard v6 by Necrym59
-- DESCRIPTION: Will create a leaderboard of highest scores using a designated UserGlobal.
-- DESCRIPTION: Attach to an object set AlwaysActive=On.
-- DESCRIPTION: [@@CURRENT_SCORE_GLOBAL$=""(0=globallist)] User Global used for current game score (eg; MyPoints)
-- DESCRIPTION: [@@TOTAL_SCORE_GLOBAL$=""(0=globallist)] User Global used for total accrued game score (eg; MyTotalPoints)
-- DESCRIPTION: [@@HISCORE1_GLOBAL$=""(0=globallist)] User Global for High Score 1
-- DESCRIPTION: [@@HISCORE2_GLOBAL$=""(0=globallist)] User Global for High Score 2
-- DESCRIPTION: [@@HISCORE3_GLOBAL$=""(0=globallist)] User Global for High Score 3
-- DESCRIPTION: [@@HISCORE4_GLOBAL$=""(0=globallist)] User Global for High Score 4
-- DESCRIPTION: [@@HISCORE5_GLOBAL$=""(0=globallist)] User Global for High Score 5
-- DESCRIPTION: [@@LEADERBOARD_HUD$=""(0=hudscreenlist)] eg: "HUD Screen 3"
-- DESCRIPTION: [DISPLAY_AT_START!=0] will display hud at startup.
-- DESCRIPTION: [RESET_TOTAL_SCORE!=0] Set ON to reset the total accrued game score total to 0

local leaderboard 				= {}
local current_score_global		= {}
local total_score_global		= {}
local hiscore1_global 			= {}
local hiscore2_global 			= {}
local hiscore3_global 			= {}
local hiscore4_global 			= {}
local hiscore5_global 			= {}
local leaderboard_hud			= {}
local display_at_start			= {}
local reset_total_score			= {}

local highscore1		= {}
local highscore2		= {}
local highscore3		= {}
local highscore4		= {}
local highscore5		= {}
local accumscore		= {}
local currentvalue		= {}
local startdisplay		= {}
local tempcheck		= {}
local doonce			= {}
local status 			= {}

function leaderboard_properties(e, current_score_global, total_score_global, hiscore1_global, hiscore2_global, hiscore3_global, hiscore4_global, hiscore5_global, leaderboard_hud, display_at_start, reset_total_score)
	leaderboard[e].current_score_global = current_score_global
	leaderboard[e].total_score_global = total_score_global	
	leaderboard[e].hiscore1_global = hiscore1_global
	leaderboard[e].hiscore2_global = hiscore2_global
	leaderboard[e].hiscore3_global = hiscore3_global
	leaderboard[e].hiscore4_global = hiscore4_global
	leaderboard[e].hiscore5_global = hiscore5_global
	leaderboard[e].leaderboard_hud = leaderboard_hud
	leaderboard[e].display_at_start = display_at_start
	leaderboard[e].reset_total_score = reset_total_score or 0
end 

function leaderboard_init(e)
	leaderboard[e] = {}
	leaderboard[e].current_score_global = ""
	leaderboard[e].total_score_global = ""
	leaderboard[e].hiscore1_global = ""
	leaderboard[e].hiscore2_global = ""
	leaderboard[e].hiscore3_global = ""
	leaderboard[e].hiscore4_global = ""
	leaderboard[e].hiscore5_global = ""
	leaderboard[e].leaderboard_hud = ""
	leaderboard[e].display_at_start = 0
	leaderboard[e].reset_total_score = 0	
	
	status[e] = "init"	
	currentvalue[e] = 0
	startdisplay[e] = 0
	highscore1[e] = 0
	highscore2[e] = 0
	highscore3[e] = 0
	highscore4[e] = 0
	highscore5[e] = 0
	accumscore[e] = 0
	tempcheck[e] = 0
	doonce[e] = 0
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
			accumscore[e] = file:read("*n", "*l")
			file:close() -- Close the file if it did successfully open.
			_G["g_UserGlobal['"..leaderboard[e].hiscore1_global.."']"] = highscore1[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore2_global.."']"] = highscore2[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore3_global.."']"] = highscore3[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore4_global.."']"] = highscore4[e]
			_G["g_UserGlobal['"..leaderboard[e].hiscore5_global.."']"] = highscore5[e]
			_G["g_UserGlobal['"..leaderboard[e].total_score_global.."']"] = accumscore[e]
		else
			local file = io.open("databank\\leaderboard.dat", "w")
			file:write(highscore1[e].."\n")
			file:write(highscore2[e].."\n")
			file:write(highscore3[e].."\n")
			file:write(highscore4[e].."\n")
			file:write(highscore5[e].."\n")
			file:write(accumscore[e].."\n")
			file:close() -- Close the file if new file created.
		end
		status[e] = "waiting"
	end
	if status[e] == "waiting" then
		if leaderboard[e].reset_total_score == 1 and doonce[e] == 0 then
			highscore5[e] = highscore5[e]
			highscore4[e] = highscore4[e]				
			highscore3[e] = highscore3[e]
			highscore2[e] = highscore2[e]
			highscore1[e] = highscore1[e]
			accumscore[e] = 0
			doonce[e] = 1
			status[e] = "savescores"			
		end
		if leaderboard[e].current_score_global ~= "" then
			if _G["g_UserGlobal['"..leaderboard[e].current_score_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..leaderboard[e].current_score_global.."']"] end			
			if currentvalue[e] > tempcheck[e] then
				accumscore[e] = accumscore[e] + 1
				if accumscore[e] > highscore1[e] then					
					highscore5[e] = highscore4[e]
					highscore4[e] = highscore3[e]				
					highscore3[e] = highscore2[e]
					highscore2[e] = highscore1[e]
					highscore1[e] = accumscore[e]
					accumscore[e] = accumscore[e]
				elseif accumscore[e] > highscore2[e] then
					highscore5[e] = highscore4[e]
					highscore4[e] = highscore3[e]				
					highscore3[e] = highscore2[e]
					highscore2[e] = accumscore[e]
					highscore1[e] = highscore1[e]
					accumscore[e] = accumscore[e]
				elseif accumscore[e] > highscore3[e] then
					highscore5[e] = highscore4[e]
					highscore4[e] = highscore3[e]				
					highscore3[e] = accumscore[e]
					highscore2[e] = highscore2[e]
					highscore1[e] = highscore1[e]
					accumscore[e] = accumscore[e]
				elseif accumscore[e] > highscore4[e] then
					highscore5[e] = highscore4[e]
					highscore4[e] = accumscore[e]				
					highscore3[e] = highscore3[e]
					highscore2[e] = highscore2[e]
					highscore1[e] = highscore1[e]
					accumscore[e] = accumscore[e]
				elseif accumscore[e] > highscore5[e] then
					highscore5[e] = accumscore[e]
					highscore4[e] = highscore4[e]			
					highscore3[e] = highscore3[e]
					highscore2[e] = highscore2[e]
					highscore1[e] = highscore1[e]
					accumscore[e] = accumscore[e]
				else
					highscore5[e] = highscore5[e]
					highscore4[e] = highscore4[e]				
					highscore3[e] = highscore3[e]
					highscore2[e] = highscore2[e]
					highscore1[e] = highscore1[e]
					accumscore[e] = accumscore[e]
				end
				_G["g_UserGlobal['"..leaderboard[e].hiscore1_global.."']"] = highscore1[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore2_global.."']"] = highscore2[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore3_global.."']"] = highscore3[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore4_global.."']"] = highscore4[e]
				_G["g_UserGlobal['"..leaderboard[e].hiscore5_global.."']"] = highscore5[e]
				_G["g_UserGlobal['"..leaderboard[e].total_score_global.."']"] = accumscore[e]
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
		file:write(accumscore[e].."\n")
		file:close()
		tempcheck[e] = currentvalue[e]
		status[e] = "loadscores"
	end
end	