-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Kill Count v8 by Necrym59 and Lee
-- DESCRIPTION: Creates counters for all designated Npc types killed in a level.
-- DESCRIPTION: [@@LEVEL_COUNT_USER_GLOBAL$=""(0=globallist)] User global for level kill count (eg: "MyLevelCounter").
-- DESCRIPTION: [@@TOTAL_COUNT_USER_GLOBAL$=""(0=globallist)] User global for total game kill count (eg: "MyGameCounter").
-- DESCRIPTION: [@ENTITY_ALLEGIENCE=0(0=Enemy, 1=Ally, 2=Neutral)]
-- DESCRIPTION: [@ENTITY_TYPE=1(1=All, 2=Named Only)]
-- DESCRIPTION: [ENTITY_NAME$="Rabbit"]
-- DESCRIPTION: [RESET_TOTAL_COUNT!=0] Set ON to reset the counter to 0
-- DESCRIPTION: [@@EFFECTED_USER_GLOBAL$=""(0=globallist)] User global for adjustment upon kill (eg: "MyReputation")
-- DESCRIPTION: [@EFFECT_STYLE=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [EFFECT_VALUE=0(0,100)] to increment/deduct the User Global by.


local lower = string.lower
local killcount					= {}
local level_count_user_global	= {}
local total_count_user_global	= {}
local entity_allegience			= {}
local entity_type				= {}
local entity_name				= {}
local reset_total_count			= {}
local effected_user_global 		= {}
local effect_style 				= {}
local effect_value 				= {}

local pEntalive 	= {}
local pEnttotal		= {}
local status	 	= {}
local checktime		= {}
local currcount 	= {}
local currentvalue	= {}
local currentvalue2	= {}
--local doonce		= {}
local reset			= {}
local tempcheck		= {}

function killcount_properties(e, level_count_user_global, total_count_user_global, entity_allegience, entity_type, entity_name, reset_total_count, effected_user_global, effect_style, effect_value)
	killcount[e].level_count_user_global = level_count_user_global
	killcount[e].total_count_user_global = total_count_user_global
	killcount[e].entity_allegience = entity_allegience or 0
	killcount[e].entity_type = entity_type or 1
	killcount[e].entity_name = lower(entity_name)
	killcount[e].reset_total_count = reset_total_count or 0
	killcount[e].effected_user_global = effected_user_global
	killcount[e].effect_style = effect_style
	killcount[e].effect_value = effect_value	
end

function killcount_init(e)
	killcount[e] = {}
	killcount[e].level_count_user_global = ""
	killcount[e].total_count_user_global = ""	
	killcount[e].entity_allegience = 0
	killcount[e].entity_type = 1
	killcount[e].entity_name = ""
	killcount[e].reset_total_count = 0
	killcount[e].effected_user_global = ""
	killcount[e].effect_style = 1
	killcount[e].effect_value = 0	
	
	pEntalive[e] = 0
	pEnttotal[e] = 0
	checktime[e] = math.huge
	currcount[e] = 0
	currentvalue[e] = 0
	currentvalue2[e] = 0	
	--doonce[e] = 0
	reset[e] = 0
	tempcheck[e] = 0
	status[e] = 'init'
end

leelee = 0

function killcount_main(e)

	if status[e] == 'init' then
		if killcount[e].reset_total_count == 1 and reset[e] == 0 then
			if killcount[e].total_count_user_global ~= "" then
				_G["g_UserGlobal['"..killcount[e].total_count_user_global.."']"] = 0
			end
			reset[e] = 1
		end	
		status[e] = 'check'
	end

	if status[e] == 'check' then
		pEnttotal[e] = 0
		pEntalive[e] = 0
		for a = 1, g_EntityElementMax do
			if a ~= nil and g_Entity[a] ~= nil then
				if killcount[e].entity_type == 1 then
					local allegiance = GetEntityAllegiance(a)
					-- this allows the hostile_zone script to toggle between enemy(0) and neutral(2) without affecting alive count
					if allegiance == killcount[e].entity_allegience then --or allegiance == 2 then
						pEnttotal[e] = pEnttotal[e] + 1
						if g_Entity[a]['health'] >= 1 then
							pEntalive[e] = pEntalive[e] + 1
						end
					end
				end
				if killcount[e].entity_type == 2 then
					if lower(GetEntityName(a)) == lower(killcount[e].entity_name) then
						local allegiance = GetEntityAllegiance(a)
						-- this allows the hostile_zone script to toggle between enemy(0) and neutral(2) without affecting alive count
						if allegiance == killcount[e].entity_allegience then --or allegiance == 2 then
							pEnttotal[e] = pEnttotal[e] + 1
							if g_Entity[a]['health'] >= 1 then
								pEntalive[e] = pEntalive[e] + 1
							end
						end
					end
				end
			end			
		end
		--if pEnttotal[e] < pEntalive[e] then doonce[e] = 0 end		
		checktime[e] = g_Time + 1000
		status[e] = 'run'
	end

	-- debug view of ent total vs alive
	-- PromptGuruMeditation(status[e].." "..pEnttotal[e].." "..pEntalive[e])

	if status[e] == 'run' then
		--enemies can be created and destroyed by other means, so pEnttotal must be a live total to keep true kill count correct
		--if doonce[e] == 0 then
		--	pEnttotal[e] = pEntalive[e]
		--	doonce[e] = 1
		--end
		currcount[e] = (pEnttotal[e]-pEntalive[e])
		if killcount[e].level_count_user_global ~= "" then
			_G["g_UserGlobal['"..killcount[e].level_count_user_global.."']"] = currcount[e]
		end
		if currcount[e] > tempcheck[e] then			
			if killcount[e].total_count_user_global ~= "" then
				if _G["g_UserGlobal['"..killcount[e].total_count_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..killcount[e].total_count_user_global.."']"] end
				_G["g_UserGlobal['"..killcount[e].total_count_user_global.."']"] = currentvalue[e] + 1
			end
			if killcount[e].effected_user_global > "" then
				if _G["g_UserGlobal['"..killcount[e].effected_user_global.."']"] ~= nil then currentvalue2[e] = _G["g_UserGlobal['"..killcount[e].effected_user_global.."']"] end
				if killcount[e].effect_style == 1 then --add				
					_G["g_UserGlobal['"..killcount[e].effected_user_global.."']"] = currentvalue2[e] + killcount[e].effect_value
				end
				if killcount[e].effect_style == 2 then --deduct				
					_G["g_UserGlobal['"..killcount[e].effected_user_global.."']"] = currentvalue2[e] - killcount[e].effect_value
				end	
			end				
			tempcheck[e] = currcount[e]
		end
		if g_Time > checktime[e] then			
			if pEntalive[e] > 0 then
				status[e] = 'check'
			end
			if pEntalive[e] == 0 then status[e] = 'end' end
		end
	end
	if status[e] == 'end' then		
		status[e] = 'stop'
		SwitchScript(e,"no_behavior_selected.lua")
	end
end