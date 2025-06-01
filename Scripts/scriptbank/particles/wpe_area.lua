-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Wicked Particle Emmitter - Area v3 
-- DESCRIPTION: Will display particle at and around this objects area.
-- DESCRIPTION: Attach to an object and activate by a linked switch or zone or set is active.
-- DESCRIPTION: [WPEFILE$="particlesbank//wpe//firearea.pe"]
-- DESCRIPTION: [IsActive!=0]
-- DESCRIPTION: [OffsetY=0] controls how far above the object the effect is displayed.
-- DESCRIPTION: <Sound0> Looped Sound Effect

local wpearea		= {}
local wpefile		= {}
local isactive		= {}
local effectid 		= {}
local framecount 	= {}
local status		= {}
local played		= {}
local sndvol		= {}

function wpe_area_properties(e, wpefile, isactive, offsety)
	wpearea[e].wpefile = wpefile
	wpearea[e].effectid = WParticleEffectLoad(wpefile)
	wpearea[e].isactive = isactive	or 0
	wpearea[e].offsety = offsety
end

function wpe_area_init(e)
	wpearea[e] = {}
	wpearea[e].wpefile = ""
	wpearea[e].effectid = ""
	wpearea[e].isactive = 0
	wpearea[e].offsety = 0
	
	status[e] = "init"
	framecount[e] = 0
	played[e] = 0
	sndvol[e] = 0
end

function wpe_area_main(e)

	if status[e] == "init" then
		if wpearea[e].isactive == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if played[e] == 0 and sndvol[e] < 100 then
			LoopSound(e,0)
			SetSoundVolume(sndvol[e])
			sndvol[e] = sndvol[e] + 0.05
			if sndvol[e] >= 100 then
				sndvol[e] = 100
				played[e] = 1
			end	
		end
		if played[e] == 1 then
			sndvol[e] = (1000-GetPlayerDistance(e))/10
			LoopSound(e,0)
			SetSoundVolume(sndvol[e])
		end
		Text(50,50,3,sndvol[e])
		WParticleEffectPosition(wpearea[e].effectid,g_Entity[e]['x'],g_Entity[e]['y']+wpearea[e].offsety,g_Entity[e]['z'])
		WParticleEffectVisible(wpearea[e].effectid,1)
		WParticleEffectAction(wpearea[e].effectid,3)
	end
	if g_Entity[e]['activated'] == 0 then
		sndvol[e] = 0
		played[e] = 0
		WParticleEffectVisible(wpearea[e].effectid,0)
		WParticleEffectAction(wpearea[e].effectid,2)
	end	
end
