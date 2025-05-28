-- No_Kill v3 by Necrym59
-- DESCRIPTION: Global behavior will kill the player and/or add/deduct from a user global if they kill a NPC of the designated type. Attach to an object, set AlwaysActive=ON.
-- DESCRIPTION: [@NPC_TYPE=1(0=Enemy, 1=Ally, 2=Neutral, 3=Ally+Neutral)]
-- DESCRIPTION: [@KILL_EFFECT=1(1=None, 2=Kill Player)]
-- DESCRIPTION: [EFFECTED_USER_GLOBAL$=""] User global for adjustment upon kill (eg: "MyReputation")
-- DESCRIPTION: [@EFFECT_STYLE=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [EFFECT_VALUE=0(0,100)] to increment/deduct the User Global by.

local nokill 				= {}
local npc_type	 			= {}
local kill_effect			= {}
local effected_user_global 	= {}
local effect_style 			= {}
local effect_value 			= {}
local nklist 				= {}

local status				= {}
local currentvalue			= {}

function no_kill_properties(e, npc_type, kill_effect, effected_user_global, effect_style, effect_value)
	nokill[e].npc_type = npc_type or 1
	nokill[e].kill_effect = kill_effect or 1
	nokill[e].effected_user_global = effected_user_global
	nokill[e].effect_style = effect_style
	nokill[e].effect_value = effect_value
end 

function no_kill_init(e)
	nokill[e] = {}
	nokill[e].npc_type = 2
	nokill[e].kill_effect = 1	
	nokill[e].effected_user_global = ""
	nokill[e].effect_style = 1
	nokill[e].effect_value = 0

	nokill[e].checkdelay = 0.5
	nokill[e].timer = 0	
	status[e] = "init"
	currentvalue[e] = 0
end 

function no_kill_main(e)

	if status[e] == "init" then
		for n = 1, g_EntityElementMax do
			if n ~= nil and g_Entity[n] ~= nil then
				local allegiance = GetEntityAllegiance(n)
				if nokill[e].npc_type == 0 then 
					if allegiance == 0 then 
						table.insert(nklist,n)
					end
				end
				if nokill[e].npc_type == 1 then 
					if allegiance == 1 then 
						table.insert(nklist,n)
					end
				end
				if nokill[e].npc_type == 2 then 
					if allegiance == 2 then 
						table.insert(nklist,n)
					end
				end
				if nokill[e].npc_type == 3 then 
					if allegiance == 1 or allegiance == 2 then 
						table.insert(nklist,n)
					end
				end				
			end
		end
		status[e] = "endinit"
	end
	
	if g_PlayerGunFired > 0 then 
		for a,b in pairs (nklist) do
			if g_Entity[b].health < 1 then 
				if nokill[e].kill_effect == 1 then 
					table.remove(nklist,a)
					if nokill[e].effected_user_global > "" then
						if _G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] end
						if nokill[e].effect_style == 1 then --add				
							_G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] = currentvalue[e] + nokill[e].effect_value   --/2
						end
						if nokill[e].effect_style == 2 then --deduct				
							_G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] = currentvalue[e] - nokill[e].effect_value   --/2
						end	
					end					
				end	
				if nokill[e].kill_effect == 2 then 
					HurtPlayer(e,g_PlayerHealth)
					table.remove(nklist,a)
					if nokill[e].effected_user_global > "" then
						if _G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] end
						if nokill[e].effect_style == 1 then --add				
							_G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] = currentvalue[e] + nokill[e].effect_value   --/2
						end
						if nokill[e].effect_style == 2 then --deduct				
							_G["g_UserGlobal['"..nokill[e].effected_user_global.."']"] = currentvalue[e] - nokill[e].effect_value   --/2
						end	
					end
				end
				break 
			end 
		end 
	else 
		nokill[e].timer = nokill[e].timer + GetElapsedTime()
		if nokill[e].timer > nokill[e].checkdelay then 
			nokill[e].timer = 0
			for a,b in pairs (nklist) do 
				if g_Entity[b].health < 1 then 
					table.remove(nklist,a)
				end 
			end 
		end 	
	end
end 