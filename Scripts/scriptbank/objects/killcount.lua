-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Kill Count v4 by Necrym59
-- DESCRIPTION: Creates counters for all enemies killed in level and/or game.
-- DESCRIPTION: [LEVEL_COUNT_USER_GLOBAL$=""] User global for Level count (eg: "MyLevelCounter").
-- DESCRIPTION: [GAME_COUNT_USER_GLOBAL$=""] User global for Game count (eg: "MyGameCounter").

local killcount					= {}
local level_count_user_global	= {}
local game_count_user_global	= {}

local pEntalive 	= {}
local pEnttotal		= {}
local status	 	= {}
local checktime		= {}
local currcount 	= {}
local currentval	= {}
local doonce		= {}

function killcount_properties(e, level_count_user_global, game_count_user_global)
	killcount[e].level_count_user_global = level_count_user_global
	killcount[e].game_count_user_global = game_count_user_global
end 

function killcount_init(e)
	killcount[e] = {}
	killcount[e].level_count_user_global = ""
	killcount[e].game_count_user_global = ""
	
	currentval[e] = 0
	pEntalive[e] = 0
	pEnttotal[e] = 0
	checktime[e] = math.huge
	currcount[e] = 0
	doonce[e] = 0
	status[e] = 'init'
end

function killcount_main(e)

	if status[e] == 'init' then
		if killcount[e].level_count_user_global ~= "" then
			_G["g_UserGlobal['"..killcount[e].level_count_user_global.."']"] = 0
		end
		status[e] = 'check'
	end

	if status[e] == 'check' then
		pEntalive[e] = 0
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then 
				local allegiance = GetEntityAllegiance(a)				
				if allegiance == 0 then
					if g_Entity[a]['health'] > 1 then
						pEntalive[e] = pEntalive[e] + 1
					end
				end
			end				
		end
		if doonce[e] == 0 then
			pEnttotal[e] = pEntalive[e]
			doonce[e] = 1
		end
		currcount[e] = (pEnttotal[e] - pEntalive[e])
		checktime[e] = g_Time + 1000
		status[e] = 'run'
	end
	
	if status[e] == 'run' then
		if killcount[e].level_count_user_global ~= "" then
			_G["g_UserGlobal['"..killcount[e].level_count_user_global.."']"] = currcount[e]
		end
		if g_Time > checktime[e] then		
			if pEntalive[e] > 0 then status[e] = 'check' end
			if pEntalive[e] == 0 then status[e] = 'end' end
		end	
	end
	
	if status[e] == 'end' then
		if killcount[e].game_count_user_global ~= "" then
			if _G["g_UserGlobal['"..killcount[e].game_count_user_global.."']"] ~= nil then currentval[e] = _G["g_UserGlobal['"..killcount[e].game_count_user_global.."']"] end
			_G["g_UserGlobal['"..killcount[e].game_count_user_global.."']"] = currentval[e] + currcount[e]
		end
		status[e] = 'stop'
		SwitchScript(e,"no_behavior_selected.lua")
	end
end