-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Hud Zone v3 by Necrym59
-- DESCRIPTION: Will display the designated hud screen when entering this zone.
-- DESCRIPTION: Attach to a trigger Zone
-- DESCRIPTION: [@@DISPLAY_HUD$=""(0=hudscreenlist)] eg: "Hud Screen 3"
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [LiveHud!=0]
-- DESCRIPTION: <Sound0> for when entering the zone

g_liveHudScreen				= {}
local hud_zone 				= {}
local display_hud 			= {}
local zoneheight			= {}
local spawnatstart			= {}
local livehud				= {}

local status 				= {}
local played				= {}

function hud_zone_properties(e, display_hud, zoneheight, spawnatstart, livehud)
	hud_zone[e].display_hud = display_hud
	hud_zone[e].zoneheight = zoneheight or 100
	hud_zone[e].spawnatstart = spawnatstart or 1
	hud_zone[e].livehud = livehud or 0
end

function hud_zone_init(e)
	hud_zone[e] = {}
	hud_zone[e].display_hud = ""
	hud_zone[e].zoneheight = 100
	hud_zone[e].spawnatstart = 1
	hud_zone[e].livehud = 0	

	status[e] = "init"
	played[e] = 0
	g_liveHudScreen = 0
end

function hud_zone_main(e)

	if status[e] == "init" then
		if hud_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if hud_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "hudclosed"
	end

	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+hud_zone[e].zoneheight and status[e] == "hudclosed" then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
			end
			if GetCurrentScreen() == -1 and status[e] == "hudclosed" then
				if hud_zone[e].display_hud ~= "" then
					if hud_zone[e].livehud == 0 then
						ScreenToggle(hud_zone[e].display_hud)
						g_liveHudScreen = 0
					end
					if hud_zone[e].livehud == 1 then
						g_liveHudScreen = 1
						ScreenToggle(hud_zone[e].display_hud)
					end
					status[e] = "hudopened"
				end
			else				
				-- in chest screen
			end
		end
		if g_Entity[e]['plrinzone'] ~= 1 and status[e] == "hudopened" then
			ScreenToggle("")
			played[e] = 0
			status[e] = "hudclosed"
		end	
	end
end

function hud_zone_exit(e)
end