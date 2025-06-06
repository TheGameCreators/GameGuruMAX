-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Wicked Particle Emmitter Zone v4
-- DESCRIPTION: While the player enters the zone the [WPEFILE$="particlesbank//wpe//heavy-rain3.pe"] is displayed.
-- DESCRIPTION: [ZONEHEIGHT=100] controls how far above the zone the player can be before the zone is not triggered.
-- DESCRIPTION: [DisableOutside!=1] stops particles when player exits the zone
-- DESCRIPTION: [DisableIndoors!=1] stops particles when player when indoors
-- DESCRIPTION: <Sound0> Loop Sound Effect

local wpe_zone 				= {}
local wpefile 				= {}
local effectid 				= {}
local zoneheight 			= {}
local DisableOutside		= {}
local DisableIndoors 		= {}

local status 			= {}
local framecount 		= {}
local played			= {}
local sndvol			= {}

function wpe_zone_properties(e, wpefile, zoneheight, DisableOutside, DisableIndoors)
	wpe_zone[e].wpefile = wpefile	
	wpe_zone[e].zoneheight = zoneheight
	wpe_zone[e].DisableOutside = DisableOutside or 1
	wpe_zone[e].DisableIndoors = DisableIndoors or 1
end

function wpe_zone_init(e)
	wpe_zone[e] = {}
	wpe_zone[e].wpefile = ""
	wpe_zone[e].zoneheight = 100
	wpe_zone[e].DisableOutside = 1
	wpe_zone[e].DisableIndoors = 1
	wpe_zone[e].effectid = ""	
	wpe_zone[e].status = "init"
	wpe_zone[e].framecount = 0
	
	played[e] = 0
	sndvol[e] = 0
end


function wpe_zone_main(e)
	HitObj = 0
	if wpe_zone[e].status == "init" then
		wpe_zone[e].effectid = WParticleEffectLoad(wpe_zone[e].wpefile)
		WParticleEffectVisible(wpe_zone[e].effectid,0)
		WParticleEffectAction(wpe_zone[e].effectid,2) -- WParticleEffectAction(effectid,Action) - Action =  1=Burst all 2=Pause 3=Resume 4=Restart
		WParticleEffectPosition(wpe_zone[e].effectid,g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ)
		wpe_zone[e].status = "endinit"
	end

	wpe_zone[e].framecount = wpe_zone[e].framecount + 1
	if wpe_zone[e].DisableIndoors >= 1 then
		if math.fmod(wpe_zone[e].framecount,30) == 0 then
			HitObj = IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,g_PlayerPosY+3000,g_PlayerPosZ,0)
			if HitObj > 0 then
				WParticleEffectVisible(wpe_zone[e].effectid,0)
				WParticleEffectAction(wpe_zone[e].effectid,2)
			else
				WParticleEffectVisible(wpe_zone[e].effectid,1)
				WParticleEffectAction(wpe_zone[e].effectid,3)
			end
		end
	end

	if g_Entity[e]['plrinzone'] == 1 and g_PlayerPosY > g_Entity[e]['y'] and g_PlayerPosY < g_Entity[e]['y']+wpe_zone[e].zoneheight then
		if played[e] == 0 and sndvol[e] < 100 then
			LoopSound(e,0)
			SetSoundVolume(sndvol[e])
			sndvol[e] = sndvol[e] + 0.05
			if sndvol[e] >= 100 then
				sndvol[e] = 100
				played[e] = 1
			end	
		end
		WParticleEffectPosition(wpe_zone[e].effectid,g_Entity[e]['x'],g_Entity[e]['y'],g_Entity[e]['z'])
		WParticleEffectVisible(wpe_zone[e].effectid,1)
		WParticleEffectAction(wpe_zone[e].effectid,3)
	else
		if wpe_zone[e].DisableOutside >= 1 then
			WParticleEffectVisible(wpe_zone[e].effectid,0)
			WParticleEffectAction(wpe_zone[e].effectid,2)
			StopSound(e,0)
			sndvol[e] = 0
			played[e] = 0			
		end
	end
end
