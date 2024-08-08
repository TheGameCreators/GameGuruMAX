-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Landing_zone v10 by Necrym59
-- DESCRIPTION: When aircraft vehicle enters this zone, <Sound0> will play and will activate 'g_LandingZone'
-- DESCRIPTION: variable to enable Auto-landing option in a capable vehicle.
-- DESCRIPTION: will also activate any logic linked or IfUsed entities.
-- DESCRIPTION: [BASEHEIGHT=100(1,500)] for the lowest sense level
-- DESCRIPTION: [ZONEHEIGHT=1000(1,5000)] for the highest sense level
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> - Enter zone Effect Sound

g_LandingZone			= {}
local landing_zone 		= {}
local baseheight		= {}
local zoneheight		= {}
local spawnatstart		= {}
local played 			= {}
local status			= {}
	
function landing_zone_properties(e, baseheight, zoneheight, spawnatstart)
	landing_zone[e].baseheight = baseheight
	landing_zone[e].zoneheight = zoneheight
	landing_zone[e].spawnatstart = spawnatstart
end
 
function landing_zone_init(e)
	landing_zone[e] = {}
	landing_zone[e].baseheight = 100
	landing_zone[e].zoneheight = 1000
	landing_zone[e].spawnatstart = 1
	g_LandingZone = 0
	status[e] = "init"
end
 
function landing_zone_main(e)	
	if status[e] == "init" then
		if landing_zone[e].spawnatstart == 1 then SetActivated(e,1) end
		if landing_zone[e].spawnatstart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then	
		if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y']+landing_zone[e].baseheight and g_PlayerPosY < g_Entity[e]['y']+landing_zone[e].zoneheight then
			if played[e] == 0 then
				PlaySound(e,0)
				played[e] = 1
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				g_LandingZone = 1
			end			
		end	
		if g_Entity[e]['plrinzone'] == 0 then
			played[e] = 0
			g_LandingZone = 0
		end
	end
	if g_Entity[e]['activated'] == 0 then
		played[e] = 0
		g_LandingZone = 0
	end
	if g_Entity[e]['plrinzone'] == 0 then
		played[e] = 0
		g_LandingZone = 0
	end
end
 
function landing_zone_exit(e)
end