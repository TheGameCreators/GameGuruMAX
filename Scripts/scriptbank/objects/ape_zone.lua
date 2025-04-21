-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Advanced Particle Emmitter Zone v1
-- DESCRIPTION: Advanced Particle Emmitter Zone
-- DESCRIPTION: While the player is within the zone the [APEFILE$="particlesbank//advanced//heavy-rain3.pe"] is displayed.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [DisableOutside!=1]
-- DESCRIPTION: [DisableIndoor!=1]

local ape_zone 			= {}
local apefile 			= {}
local effectid 			= {}
local zoneheight 		= {}
local DisableOutside 	= {}
local DisableIndoor 	= {}

function ape_zone_properties(e, apefile, zoneheight, DisableOutside, DisableIndoor)
	ape_zone[e].apefile = apefile	
	ape_zone[e].zoneheight = zoneheight
	ape_zone[e].outsidezone = DisableOutside or 1
	ape_zone[e].disableindoor = DisableIndoor or 1
end

function ape_zone_init(e)
	ape_zone[e] = {}
	ape_zone[e].apefile = ""
	ape_zone[e].zoneheight = 100
	ape_zone[e].outsidezone = 1
	ape_zone[e].disableindoor = 1
	ape_zone[e].effectid = ""	
	ape_zone[e].status = "init"
	ape_zone[e].framecount = 0
end


function ape_zone_main(e)
	HitObj = 0
	if ape_zone[e].status == "init" then
		ape_zone[e].effectid = WParticleEffectLoad(ape_zone[e].apefile)
		WParticleEffectVisible(ape_zone[e].effectid,0)
		WParticleEffectAction(ape_zone[e].effectid,2) -- WParticleEffectAction(effectid,Action) - Action =  1=Burst all 2=Pause 3=Resume 4=Restart
		WParticleEffectPosition(ape_zone[e].effectid,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
		ape_zone[e].status = "endinit"
	end

	ape_zone[e].framecount = ape_zone[e].framecount + 1
	if ape_zone[e].disableindoor >= 1 then
		if math.fmod(ape_zone[e].framecount,30) == 0 then
			HitObj = IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,g_PlayerPosY+3000,g_PlayerPosZ,0)
			if HitObj > 0 then
				WParticleEffectVisible(ape_zone[e].effectid,0)
				WParticleEffectAction(ape_zone[e].effectid,2)
			else
				WParticleEffectVisible(ape_zone[e].effectid,1)
				WParticleEffectAction(ape_zone[e].effectid,3)
			end
		end
	end

	if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+ape_zone[e].zoneheight then
		WParticleEffectPosition(ape_zone[e].effectid,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
		WParticleEffectVisible(ape_zone[e].effectid,1)
		WParticleEffectAction(ape_zone[e].effectid,3)
	else
		if ape_zone[e].outsidezone >= 1 then
			WParticleEffectVisible(ape_zone[e].effectid,0)
			WParticleEffectAction(ape_zone[e].effectid,2)
		end
	end	
end
