-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Affected Object v6 by Necrym59
-- DESCRIPTION: The player user_globals can be affected by distance from this object
-- DESCRIPTION: Attach to an object. Set AlwaysActive=ON
-- DESCRIPTION: [IN_RANGE_TEXT$="In range of Affecting Object"]
-- DESCRIPTION: [EFFECT_RANGE=500(1,3000)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [EFFECT_MULTIPLIER=1(1,10)] per [SECONDS=1(1,60)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyGlobal"]
-- DESCRIPTION: [@HEALTH_AFFECTED=1(1=None, 2=Add, 3=Deduct)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this object
-- DESCRIPTION: <Sound0> - Loop Zone Effect Sound

local gaobject 				= {}
local in_range_text 		= {}
local effect_range			= {}
local effect 				= {}
local effect_multiplier 	= {}
local seconds 				= {}
local user_global_affected	= {}
local health_affected		= {}
local SpawnAtStart			= {}

local rangedeffect			= {}
local currentvalue			= {}
local status				= {}

function global_affected_object_properties(e, in_range_text, effect_range, effect, effect_multiplier, seconds, user_global_affected, health_affected, SpawnAtStart)
	gaobject[e].in_range_text = in_range_text
	gaobject[e].effect_range = effect_range
	gaobject[e].effect = effect	
	gaobject[e].effect_multiplier = effect_multiplier
	gaobject[e].seconds = seconds
	gaobject[e].user_global_affected = user_global_affected
	gaobject[e].health_affected = health_affected or 1
	gaobject[e].SpawnAtStart = 1
end
 
function global_affected_object_init(e)
	gaobject[e] = {}
	gaobject[e].in_range_text = "In range of Affecting Object"
	gaobject[e].effect_range = 500
	gaobject[e].effect = 1	
	gaobject[e].effect_multiplier = 1
	gaobject[e].seconds = 1	
	gaobject[e].user_global_affected = ""
	gaobject[e].health_affected = 1
	gaobject[e].SpawnAtStart = 1	
	currentvalue[e] = 0
	rangedeffect[e] = 0
	status[e] = "init"
	StartTimer(e)
end

function global_affected_object_main(e)	
	
	if status[e] == "init" then
		if gaobject[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if gaobject[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	rangedeffect[e] = gaobject[e].effect_range/PlayerDist
		
	if g_Entity[e]['activated'] == 1 then	
		
		if PlayerDist < gaobject[e].effect_range then
			Prompt(gaobject[e].in_range_text)
			if gaobject[e].effect == 1 then --Add
				LoopSound(e,0)
				if GetTimer(e) > (gaobject[e].seconds * 1000) then
					if gaobject[e].health_affected == 2 then
						SetPlayerHealth(g_PlayerHealth + (rangedeffect[e]*gaobject[e].effect_multiplier))
						if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
						SetPlayerHealthCore(g_PlayerHealth)	
					end
					if gaobject[e].health_affected == 3 then
						SetPlayerHealth(g_PlayerHealth - (rangedeffect[e]*gaobject[e].effect_multiplier))
						if g_PlayerHealth <= 0 then g_PlayerHealth = 0 end
						SetPlayerHealthCore(g_PlayerHealth)	
					end						
					if gaobject[e].user_global_affected ~= "" then 
						if _G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] end
						if currentvalue[e] + (rangedeffect[e]*gaobject[e].effect_multiplier) < 100 then
							_G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] = currentvalue[e] + (rangedeffect[e]*gaobject[e].effect_multiplier)
						else
							_G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] = 100
						end
					end				
					StartTimer(e)
				end			
			end		
			if gaobject[e].effect == 2 then --Deduct
				LoopSound(e,0)
				if GetTimer(e) > (gaobject[e].seconds * 1000) then
					if gaobject[e].health_affected == 2 then
						SetPlayerHealth(g_PlayerHealth + (rangedeffect[e]*gaobject[e].effect_multiplier))
						if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
						SetPlayerHealthCore(g_PlayerHealth)	
					end
					if gaobject[e].health_affected == 3 then
						SetPlayerHealth(g_PlayerHealth - (rangedeffect[e]*gaobject[e].effect_multiplier))
						if g_PlayerHealth <= 0 then g_PlayerHealth = 0 end
						SetPlayerHealthCore(g_PlayerHealth)	
					end	
					if gaobject[e].user_global_affected ~= "" then 
						if _G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] end						
						if currentvalue[e] - (rangedeffect[e]*gaobject[e].effect_multiplier) > 0 then
							_G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] = currentvalue[e] - (rangedeffect[e]*gaobject[e].effect_multiplier)
						else
							_G["g_UserGlobal['"..gaobject[e].user_global_affected.."']"] = 0
						end	
					end
					StartTimer(e)
				end			
			end	
		end
	end
end
