-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Advanced Particle - Rain v1 
-- DESCRIPTION: Advanced Particle Effect - Rain
-- DESCRIPTION: Attach to an object set Always Active = ON
-- DESCRIPTION: [APEFILE$="particlesbank//advanced//heavy-rain3.pe"]
-- DESCRIPTION: [IsActive!=0]
-- DESCRIPTION: <Sound0> Rain Sound Effect.

local rain			= {}
local apefile		= {}
local isactive		= {}
local rainid 		= {}
local framecount 	= {}
local status		= {}

function ape_rain_properties(e, apefile,isactive)
	rain[e].apefile = apefile
	rain[e].rainid = WParticleEffectLoad(apefile)
	rain[e].isactive = isactive	or 0
end

function ape_rain_init(e)
	rain[e] = {}
	rain[e].apefile = ""
	rain[e].rainid = ""
	rain[e].isactive = 0
	
	status[e] = "init"
	framecount[e] = 0
end

function ape_rain_main(e)

	if status[e] == "init" then
		if rain[e].isactive == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		-- PE: Hide when indoor.
		framecount[e] = framecount[e] + 1
		if math.fmod(framecount[e],30) == 0 then
			HitObj = IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,g_PlayerPosY+3000,g_PlayerPosZ,0)
			if HitObj > 0 then
				WParticleEffectVisible(rain[e].rainid,0)
				WParticleEffectAction(rain[e].rainid,2)
			else
				WParticleEffectVisible(rain[e].rainid,1)
				WParticleEffectAction(rain[e].rainid,3)
			end
		end
	end
	if g_Entity[e]['activated'] == 0 then
		WParticleEffectVisible(rain[e].rainid,0)
		WParticleEffectAction(rain[e].rainid,3)
	end	
end
