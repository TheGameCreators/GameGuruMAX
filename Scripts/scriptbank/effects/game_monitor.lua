-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Game Monitor v3 by Necrym59
-- DESCRIPTION: Will monitor a User Global Value and launch the designated game action.
-- DESCRIPTION: Attach to an object set AlwaysActive=On.
-- DESCRIPTION: [@@USER_GLOBAL$=""(0=globallist)] User Global used to monitor (eg; MyPoints)
-- DESCRIPTION: [TRIGGER_VALUE=0] Value to trigger action
-- DESCRIPTION: [@TRIGGER_ACTION=1(1=Win Game, 2=Lose Game, 3=Go To Level)]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic, 2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded.

local gamemonitor 		= {}
local user_global		= {}
local trigger_value		= {}
local trigger_action	= {}

local currentvalue		= {}
local status 			= {}
local checktimer		= {}

function game_monitor_properties(e, user_global, trigger_value, trigger_action)
	gamemonitor[e].user_global = user_global
	gamemonitor[e].trigger_value = trigger_value
	gamemonitor[e].trigger_action = trigger_action	
end 

function game_monitor_init(e)
	gamemonitor[e] = {}
	gamemonitor[e].user_global = ""
	gamemonitor[e].trigger_value = 0
	gamemonitor[e].trigger_action = trigger_action
	
	status[e] = "init"
	currentvalue[e] = 0
	checktimer[e] = math.huge
end

function game_monitor_main(e)
	if status[e] == "init" then
		checktimer[e] = g_Time + 100
		status[e] = "monitor"
	end
	
	if status[e] == "monitor" then
		if gamemonitor[e].user_global > "" then
			if _G["g_UserGlobal['"..gamemonitor[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..gamemonitor[e].user_global.."']"] end
			if g_Time > checktimer[e] then
				if currentvalue[e] >= gamemonitor[e].trigger_value then	
					if gamemonitor[e].trigger_action == 1 then
						WinGame()
					end
					if gamemonitor[e].trigger_action == 2 then
						LoseGame()
					end
					if gamemonitor[e].trigger_action == 3 then
						JumpToLevelIfUsedEx(e,0)
					end					
				end
				checktimer[e] = g_Time + 100
			end
		end	
	end
end	