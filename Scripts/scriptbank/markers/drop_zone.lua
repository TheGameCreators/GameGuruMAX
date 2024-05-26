-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Drop Zone v4 by Necrym59
-- DESCRIPTION: The player receive the specified fall damage when dropping from within zone area and within its given height.
-- DESCRIPTION: [DAMAGE_REDUCTION=1(1,101)](101 = Nil Damage)
-- DESCRIPTION: [ZONEHEIGHT=200(1,1000)] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SPAWN_AT_START!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> plays a sound when entering the zone.

local drop_zone 		= {}
local damage_reduction 	= {}
local zoneheight 		= {}
local spawn_at_start	= {}
local played 			= {}
local doonce 			= {}
local status 			= {}
local hfsett			= {}

function drop_zone_properties(e, damage_reduction, zoneheight, spawn_at_start)
	drop_zone[e].damage_reduction = damage_reduction
	drop_zone[e].zoneheight = zoneheight or 100
	drop_zone[e].spawn_at_start = 1
end

function drop_zone_init(e)
	drop_zone[e] = {}
	drop_zone[e].damage_reduction = 1
	drop_zone[e].zoneheight = 100
	drop_zone[e].spawn_at_start = 1
	status[e] = "init"
	played[e] = 0
	doonce[e] = 0
end

function drop_zone_main(e)

	if status[e] == "init" then
		if drop_zone[e].spawn_at_start == 1 then SetActivated(e,1) end
		if drop_zone[e].spawn_at_start == 0 then SetActivated(e,0) end
		if drop_zone[e].zoneheight == nil then drop_zone[e].zoneheight = 100 end
		if drop_zone[e].damage_reduction == 101 then drop_zone[e].damage_reduction = 0 end
		hfsett[e] = GetGamePlayerControlHurtFall()
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		if g_Entity[e]['plrinzone']==1 and g_PlayerPosY+35 > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y'] + drop_zone[e].zoneheight then
			SetGamePlayerControlHurtFall(drop_zone[e].damage_reduction)
			if played[e] == 0 then
				PlaySound(e,0)				
				played[e] = 1
			end
		else
			SetGamePlayerControlHurtFall(hfsett[e])
		end
	end
end

function drop_zone_exit(e)
end