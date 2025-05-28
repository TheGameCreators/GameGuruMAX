-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Toxic Zone v21 by Necrym59
-- DESCRIPTION: The player or npc will be effected with health loss while in this Zone unless using protection
-- DESCRIPTION: Attach to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Toxic Zone use protection"]
-- DESCRIPTION: [@EFFECT=1(1=Gas, 2=Radiation)]
-- DESCRIPTION: [DAMAGE=1(0,1000)] per second
-- DESCRIPTION: Zone Height [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [@TOXIC_TO_NPC=1(1=Yes, 2=No)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] (eg: MyUserGlobal)
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [@GLOBAL_AFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: <Sound0> - Zone Effect Sound
-- DESCRIPTION: <Sound1> - Pain Sound

g_toxiczone = {}
g_gasmask_on = {}
g_radsuit_on = {}

local toxiczone 			= {}
local prompt_text 			= {}
local effect 				= {}
local damage 				= {}
local zoneheight			= {}
local toxic_to_npc			= {}
local user_global_affected	= {}
local global_affect			= {}
local spawnatstart			= {}
local currentvalue			= {}
local doonce				= {}
local status				= {}
local EntityID				= {}
local EntityAL				= {}

function toxiczone_properties(e, prompt_text, effect, damage, zoneheight, toxic_to_npc, user_global_affected, spawnatstart, global_affect)
	toxiczone[e].prompt_text = prompt_text
	toxiczone[e].effect = effect
	toxiczone[e].damage = damage
	toxiczone[e].zoneheight = zoneheight or 100
	toxiczone[e].toxic_to_npc = toxic_to_npc
	toxiczone[e].user_global_affected = user_global_affected
	toxiczone[e].spawnatstart = spawnatstart or 1
	toxiczone[e].global_affect = global_affect	
end

function toxiczone_init(e)
	toxiczone[e] = {}
	toxiczone[e].prompt_text = "In Toxic Zone use protection"
	toxiczone[e].effect = 1
	toxiczone[e].damage = 1
	toxiczone[e].zoneheight = 100
	toxiczone[e].toxic_to_npc = 1
	toxiczone[e].user_global_affected = ""
	toxiczone[e].spawnatstart = 1
	toxiczone[e].global_affect = 1		
	currentvalue[e] = 0
	doonce[e] = 0
	g_gasmask_on = 0
	g_radsuit_on = 0
	status[e] = "init"
	StartTimer(e)
	EntityID[e] = nil
	EntityAL[e] = nil
	--g_Entity[e]['entityinzone'] = -1
end

function toxiczone_main(e)

	if status[e] == "init" then
		if toxiczone[e].spawnatstart == 1 then SetActivated(e,1) end
		if toxiczone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+toxiczone[e].zoneheight then
			if toxiczone[e].effect == 1 then	--Health Loss
				if g_gasmask_on ~= 1 then PromptDuration(toxiczone[e].prompt_text,3000) end
				g_toxiczone = 'gas'
				LoopSound(e,0)
				if GetTimer(e) > 3000 then
					if g_gasmask_on ~= 1 then
						PlaySound(e,1)
						SetPlayerHealth(g_PlayerHealth - toxiczone[e].damage)
						SetPlayerHealthCore(g_PlayerHealth)
						if g_PlayerHealth <= 0 then LoseGame() end						
					end
					if toxiczone[e].user_global_affected ~= "" and toxiczone[e].global_affect == 1 then
						if _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] = currentvalue[e] + toxiczone[e].damage
					end
					if toxiczone[e].user_global_affected ~= "" and toxiczone[e].global_affect == 2 then
						if _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] = currentvalue[e] - toxiczone[e].damage
					end
					StartTimer(e)
					doonce[e] = 0
				end
			end
			if toxiczone[e].effect == 2 then	--Radiation
				if g_radsuit_on ~= 1 then PromptDuration(toxiczone[e].prompt_text,3000) end
				g_toxiczone = 'radiation'
				LoopSound(e,0)
				if GetTimer(e) > 3000 then
					if g_radsuit_on ~= 1 then
						PlaySound(e,1)
						SetPlayerHealth(g_PlayerHealth - toxiczone[e].damage)
						SetPlayerHealthCore(g_PlayerHealth)
						if g_PlayerHealth <= 0 then LoseGame() end
					end
					if toxiczone[e].user_global_affected ~= "" and toxiczone[e].global_affect == 1 then
						if _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] = currentvalue[e] + toxiczone[e].damage
					end
					if toxiczone[e].user_global_affected ~= "" and toxiczone[e].global_affect == 2 then
						if _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..toxiczone[e].user_global_affected.."']"] = currentvalue[e] - toxiczone[e].damage
					end
					StartTimer(e)
					doonce[e] = 0
				end
			end
		end

		if g_Entity[e]['plrinzone'] == 0 then
			StopSound(e,0)
			StopSound(e,1)
			doonce[e] = 0
			g_toxiczone = ""
		end

		GetEntityInZoneWithFilter(e,2)
		EntityID[e] = g_Entity[e]['entityinzone']
		EntityAL[e] = GetEntityAllegiance(EntityID[e])
		if g_Entity[e]['entityinzone'] > 0 and EntityID[e] > 0 and EntityAL[e] ~= -1 and g_Entity[EntityID[e]]['y'] > g_Entity[e]['y']-10 and g_Entity[EntityID[e]]['y'] < g_Entity[e]['y']+toxiczone[e].zoneheight then
			if GetTimer(e) > 1000 then
				if toxiczone[e].toxic_to_npc == 1 and g_Entity[EntityID[e]]['health'] > 0 then SetEntityHealth(EntityID[e],g_Entity[EntityID[e]]['health']-toxiczone[e].damage) end
				StartTimer(e)
			end
		end
		if g_Entity[e]['entityinzone'] == 0 or g_Entity[e]['entityinzone'] == nil then EntityID[e] = nil end
	end
end

function toxiczone_exit(e)
end