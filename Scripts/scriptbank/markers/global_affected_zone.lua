-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Affected Zone v5 by Necrym59
-- DESCRIPTION: The player user_globals can be affected this Zone
-- DESCRIPTION: Attach to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Affected Zone"]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [AMOUNT=1(1,500)] per [SECONDS=1(1,60)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyGlobal"]
-- DESCRIPTION: [HEALTH_AFFECTED!=0]
-- DESCRIPTION: Zone Height [ZoneHeight=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> - Loop Zone Effect Sound

local gazone 				= {}
local prompt_text 			= {}
local effect 				= {}
local amount 				= {}
local seconds 				= {}
local user_global_affected	= {}
local health_affected		= {}
local ZoneHeight			= {}
local SpawnAtStart			= {}
local currentvalue			= {}
local doonce				= {}
local status				= {}

function global_affected_zone_properties(e, prompt_text, effect, amount, seconds, user_global_affected, health_affected, ZoneHeight, SpawnAtStart)
	gazone[e] = g_Entity[e]
	gazone[e].prompt_text = prompt_text
	gazone[e].effect = effect	
	gazone[e].amount = amount
	gazone[e].seconds = seconds
	gazone[e].user_global_affected = user_global_affected
	gazone[e].health_affected = health_affected
	gazone[e].ZoneHeight = ZoneHeight or 100
	gazone[e].SpawnAtStart = SpawnAtStart
end
 
function global_affected_zone_init(e)
	gazone[e] = {}
	gazone[e].prompt_text = "In Affected Zone"
	gazone[e].effect = 1	
	gazone[e].amount = 1
	gazone[e].seconds = 1	
	gazone[e].user_global_affected = ""
	gazone[e].health_affected = 0
	gazone[e].ZoneHeight = 100
	gazone[e].SpawnAtStart = 1	
	currentvalue[e] = 0
	status[e] = "init"
	doonce[e] = 0
	StartTimer(e)
end

function global_affected_zone_main(e)	
	gazone[e] = g_Entity[e]
	
	if status[e] == "init" then
		if gazone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if gazone[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then
	
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+gazone[e].ZoneHeight then
			if doonce[e] < 5 then
				PromptDuration(gazone[e].prompt_text,3000)
			end	
			if gazone[e].effect == 1 then --Add
				LoopSound(e,0)
				if GetTimer(e) > (gazone[e].seconds * 1000) then
					if gazone[e].health_affected == 1 then
						SetPlayerHealth(g_PlayerHealth + gazone[e].amount)
					end	
					if gazone[e].user_global_affected ~= "" then 
						if _G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] end
						if currentvalue[e] + gazone[e].amount < 100 then
							_G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] = currentvalue[e] + gazone[e].amount
						else
							_G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] = 100
						end
					end
					doonce[e] = doonce[e] + 1
					StartTimer(e)
				end			
			end		
			if gazone[e].effect == 2 then --Deduct
				LoopSound(e,0)
				if GetTimer(e) > (gazone[e].seconds * 1000) then
					if gazone[e].health_affected == 1 then
						SetPlayerHealth(g_PlayerHealth - gazone[e].amount)
					end	
					if gazone[e].user_global_affected ~= "" then 
						if _G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] end
						if currentvalue[e] - gazone[e].amount > 0 then
							_G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] = currentvalue[e] - gazone[e].amount
						else
							_G["g_UserGlobal['"..gazone[e].user_global_affected.."']"] = 0
						end	
					end
					doonce[e] = doonce[e] + 1
					StartTimer(e)
				end			
			end	
		end	

		if g_Entity[e]['plrinzone'] == 0 then
			StopSound(e,0)
			doonce[e] = 0
		end
	end
end
 
function global_affected_zone_exit(e)	
end

