-- FreezePlayer v4 
-- DESCRIPTION: When a player enters zone will freeze the player, and stay in a frozen state for
-- DESCRIPTION: [FREEZETIME=3] seconds. Set the [ZONEHEIGHT=100(1,500)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: [MultiTrigger!=1] if unchecked will destroy this zone after use
-- DESCRIPTION: <Sound0> Sound to play when freezing
-- DESCRIPTION: <Sound1> Sound to play when unfreezing

local freeze 		= {}
local freezetime 	= {}
local zoneheight	= {}
local SpawnAtStart	= {}
local MultiTrigger	= {}

local frozenmode	= {}
local frozentime	= {}
local played		= {}
local status		= {}


function FreezePlayer_properties(e, freezetime, zoneheight, SpawnAtStart, MultiTrigger)
	freeze[e] = g_Entity[e]
	freeze[e].freezetime = freezetime
	freeze[e].zoneheight = zoneheight or 100
	freeze[e].SpawnAtStart = SpawnAtStart
	freeze[e].MultiTrigger = MultiTrigger	
end

function FreezePlayer_init(e)
	freeze[e] = {}
	freeze[e].freezetime = 3
	freeze[e].zoneheight = 100
	freeze[e].SpawnAtStart = 1
	freeze[e].MultiTrigger = 1
		
	frozenmode[e] = 0
	frozentime[e] = math.huge	
	played[e] = 0	
	status[e] = "init"
end

function FreezePlayer_main(e)
	freeze[e] = g_Entity[e]
	if status[e] == "init" then
		if freeze[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if freeze[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e].activated == 1 then
		if g_Entity[e].plrinzone ==1 and g_PlayerHealth > 0 and g_PlayerPosY > g_Entity[e].y and g_PlayerPosY < g_Entity[e].y + freeze[e].zoneheight then
			if freeze[e].freezetime ~= nil and frozenmode[e] == 0 then
				SetCameraOverride(3)
				frozentime[e] = g_Time + (freeze[e].freezetime * 1000)			
				PlaySound(e,0)
				frozenmode[e] = 1
			end
			if g_Time > frozentime[e] and frozenmode[e] == 1 then
				if played[e] == 0 then 
					PlaySound(e,1)
					played[e] = 1
				end	
				SetCameraOverride(0)				
				if freeze[e].MultiTrigger == 0 then Destroy(e) end
			end
		end
		if g_Entity[e].plrinzone == 0 then
			frozenmode[e] = 0
			played[e] = 0
		end	
	end	
end