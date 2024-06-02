-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Disarm Zone v3
-- DESCRIPTION: Will allow to disarm all weapons player has when entering in this zone then destroy the zone.
-- DESCRIPTION: Attach to a trigger Zone
-- DESCRIPTION: [PROMPT_TEXT$="All Weapons Removed"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> for when entering the zone

local disarm_zone 			= {}
local prompt_text 			= {}
local zoneheight			= {}
local spawnatstart			= {}

local status 				= {}
local played				= {}

function disarm_zone_properties(e, prompt_text, zoneheight, spawnatstart)
	disarm_zone[e].prompt_text = prompt_text
	disarm_zone[e].zoneheight = zoneheight or 100
	disarm_zone[e].spawnatstart = spawnatstart
end

function disarm_zone_init(e)
	disarm_zone[e] = {}
	disarm_zone[e].zoneheight = 100
	disarm_zone[e].spawnatstart = 1

	status[e] = "init"
	played[e] = 0
end

function disarm_zone_main(e)

	if status[e] == "init" then
		if disarm_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if disarm_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+disarm_zone[e].zoneheight then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			for i = 1, 10 do								
				local entityindex = GetInventoryItemID("inventory:hotkeys",i)				
				SetEntityCollected(entityindex,0,0)
				Show(entityindex)
				RemovePlayerWeapon(i)
				if i >=10 then break end
			end
			Prompt(disarm_zone[e].prompt_text)
			Destroy(e)
		end
	end
end

function disarm_zone_exit(e)
end