-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hud Zone v2 by Necrym59
-- DESCRIPTION: Will display the designated hud screen when entering this zone then destroy the zone.
-- DESCRIPTION: Attach to a trigger Zone
-- DESCRIPTION: [DISPLAY_HUD$=""] eg: "HUD Screen 3"
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> for when entering the zone

local hud_zone 				= {}
local display_hud 			= {}
local zoneheight			= {}
local spawnatstart			= {}

local status 				= {}
local played				= {}
local hudtime				= {}

function hud_zone_properties(e, display_hud, zoneheight, spawnatstart)
	hud_zone[e].display_hud = display_hud
	hud_zone[e].zoneheight = zoneheight or 100
	hud_zone[e].spawnatstart = spawnatstart
end

function hud_zone_init(e)
	hud_zone[e] = {}
	hud_zone[e].display_hud = ""
	hud_zone[e].zoneheight = 100
	hud_zone[e].spawnatstart = 1

	status[e] = "init"
	played[e] = 0
	hudtime[e] = math.huge
end

function hud_zone_main(e)

	if status[e] == "init" then
		if hud_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if hud_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+hud_zone[e].zoneheight then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if hud_zone[e].display_hud ~= "" then
				ScreenToggle(hud_zone[e].display_hud)				
			end
		end
		if g_Entity[e]['plrinzone'] == 0 then Destroy(e) end		
	end
end

function hud_zone_exit(e)
end