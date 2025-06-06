-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Wicked Particle Emmitter - Rain v4 
-- DESCRIPTION: Attach to an object and activate by a linked switch or zone or set Is Active to ON.
-- DESCRIPTION: [WPEFILE$="particlesbank//wpe//heavy-rain3.pe"]
-- DESCRIPTION: [IsActive!=0] if unticked will need to be activated by switch or zone
-- DESCRIPTION: <Sound0> Rain Sound Effect

g_raining			= {}	-- For external behaviors to detect when running
local wperain		= {}
local wpefile		= {}
local isactive		= {}
local effectid 		= {}
local framecount 	= {}
local status		= {}
local played		= {}
local sndvol		= {}

function wpe_rain_properties(e, wpefile,isactive)
	wperain[e].wpefile = wpefile
	wperain[e].effectid = WParticleEffectLoad(wpefile)
	wperain[e].isactive = isactive	or 0
	WParticleEffectVisible(wperain[e].effectid,0)
	WParticleEffectAction(wperain[e].effectid,3)
end

function wpe_rain_init(e)
	wperain[e] = {}
	wperain[e].wpefile = ""
	wperain[e].effectid = ""
	wperain[e].isactive = 0
	
	status[e] = "init"
	framecount[e] = 0
	sndvol[e] = 0
end

function wpe_rain_main(e)
	if status[e] == "init" then
		if wperain[e].isactive == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		g_raining = 1

		if played[e] == 0 and sndvol[e] < 100 then
			LoopSound(e,0)
			SetSoundVolume(sndvol[e])
			sndvol[e] = sndvol[e] + 0.05
			if sndvol[e] >= 100 then
				sndvol[e] = 100
				played[e] = 1
			end	
		end

		-- PE: Hide when indoor.
		framecount[e] = framecount[e] + 1
		if math.fmod(framecount[e],30) == 0 then
			HitObj = IntersectAll(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,g_PlayerPosX,g_PlayerPosY+3000,g_PlayerPosZ,0)
			if HitObj > 0 then
				WParticleEffectVisible(wperain[e].effectid,0)
				WParticleEffectAction(wperain[e].effectid,2)
			else
				WParticleEffectVisible(wperain[e].effectid,1)
				WParticleEffectAction(wperain[e].effectid,3)
			end
		end
	end
	if g_Entity[e]['activated'] == 0 then
		g_raining = 0
		StopSound(e,0)
		played[e] = 0
		WParticleEffectVisible(wperain[e].effectid,0)
		WParticleEffectAction(wperain[e].effectid,3)
	end
	if g_Entity[e]['health'] <= 0 then
		SetPreExitValue(e,1)
	end
end

function wpe_rain_preexit(e)
	WParticleEffectVisible(wperain[e].effectid,0)
	WParticleEffectAction(wperain[e].effectid,3)
end
