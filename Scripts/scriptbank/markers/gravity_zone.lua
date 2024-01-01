-- Gravity Zone v2  Necrym59
-- DESCRIPTION: Will alter the gravity while a player is in the zone.
-- DESCRIPTION: Set the [#GRAVITY_LEVEL=-3(-20,20)]
-- DESCRIPTION: Set the [#FALL_RATE=300.0(10.0,1000.0)]
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> Looped while in zone

local gravity_zone 	= {}
local gravity_level	= {}
local fall_rate		= {}
local zoneheight	= {}
local SpawnAtStart	= {}

local played		= {}
local status		= {}


function gravity_zone_properties(e, gravity_level, fall_rate, zoneheight, SpawnAtStart)
	gravity_zone[e] = g_Entity[e]
	gravity_zone[e].gravity_level = gravity_level
	gravity_zone[e].fall_rate = fall_rate
	gravity_zone[e].zoneheight = zoneheight or 100
	gravity_zone[e].SpawnAtStart = SpawnAtStart
end

function gravity_zone_init(e)
	gravity_zone[e] = {}
	gravity_zone[e].gravity_level = -3
	gravity_zone[e].fall_rate = 300
	gravity_zone[e].zoneheight = 100
	gravity_zone[e].SpawnAtStart = 1
		
	played[e] = 0	
	status[e] = "init"
end

function gravity_zone_main(e)
	gravity_zone[e] = g_Entity[e]
	if status[e] == "init" then
		if gravity_zone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if gravity_zone[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e].activated == 1 then
		if g_Entity[e].plrinzone ==1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e].y and g_PlayerPosY < g_Entity[e].y + gravity_zone[e].zoneheight then
			SetWorldGravity(0,gravity_zone[e].gravity_level,0,gravity_zone[e].fall_rate)
			if played[e] == 0 then 
				LoopSound(e,0)
				played[e] = 1
			end	
		end
		if g_Entity[e].plrinzone == 0 then		
			SetWorldGravity(0,-20,0,0) -- Reset to normal gravity
			StopSound(e,0)
			played[e] = 0
		end	
	end	
end