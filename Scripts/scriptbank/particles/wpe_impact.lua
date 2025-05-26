-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Wicked Particle Emmitter - Impact v3 
-- DESCRIPTION: Attach to an object and activate by a linked switch or zone.
-- DESCRIPTION: [WPEFILE$="particlesbank//wpe//dirt-splash4.pe"]
-- DESCRIPTION: [IsActive!=0]
-- DESCRIPTION: <Sound0> Impact/Explosion sound

local wpeimpact 		= {}
local wpefile			= {}
local isactive			= {}
local effectid 			= {}
local impacttimer 		= {}
local hurtplayer 		= {}
local status			= {}

function wpe_impact_properties(e, wpefile, isactive)
	wpeimpact[e].wpefile = wpefile
	wpeimpact[e].effectid = WParticleEffectLoad(wpefile)
	wpeimpact[e].isactive = isactive or 0
	WParticleEffectVisible(wpeimpact[e].effectid,0)
	WParticleEffectAction(wpeimpact[e].effectid,3)	
end

function wpe_impact_init(e)
	wpeimpact[e] = {}
	wpeimpact[e].wpefile = ""
	wpeimpact[e].isactive = 0	
	wpeimpact[e].effectid = ""
	wpeimpact[e].impacttimer = math.floor(math.random(-1.5,0))
	wpeimpact[e].hurtplayer = -1	
	status[e] = "init"
end

function wpe_impact_main(e)

	if status[e] == "init" then
		if wpeimpact[e].isactive == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		wpeimpact[e].impacttimer = wpeimpact[e].impacttimer + GetElapsedTime()
		if wpeimpact[e].impacttimer > 2.5 then
			randdist = math.floor(math.random(100,1000))
			randdistx = math.random(-randdist,randdist)
			randdistz = math.random(-randdist,randdist)
			posx = randdistx + g_PlayerPosX
			posz = randdistz + g_PlayerPosZ
			theight = GetTerrainHeight(posx,posz)
			HitObj = IntersectAll(posx,theight,posz,posx,theight+2500,posz,0)
			if HitObj <= 0 then
				PlaySound(e,0)
				PositionSound(e,0,posx,theight,posz) -- (Entity,Slot,X,Y,Z)
				WParticleEffectPosition(wpeimpact[e].effectid,posx,theight,posz)
				WParticleEffectVisible(wpeimpact[e].effectid,1)
				WParticleEffectAction(wpeimpact[e].effectid,1) -- burst all
				if randdistx >= -50 and randdistx <= 50 and randdistz >= -50 and randdistz <= 50 then
					wpeimpact[e].hurtplayer = 0
				end
				wpeimpact[e].impacttimer = math.floor(math.random(-1.5,0))
			end
		end
		if wpeimpact[e].hurtplayer >= 0 then wpeimpact[e].hurtplayer = wpeimpact[e].hurtplayer + GetElapsedTime() end
		if wpeimpact[e].hurtplayer > 0.950 then --Hurt player after 950 miliseconds.
			HurtPlayer(-1,50)
			wpeimpact[e].hurtplayer = -1
		end
	end
	if g_Entity[e]['activated'] == 0 then
		StopSound(e,0)
		WParticleEffectVisible(wpeimpact[e].effectid,0)
		WParticleEffectAction(wpeimpact[e].effectid,3)
	end
	if g_Entity[e]['health'] <= 0 then
		SetPreExitValue(e,1)
	end	
end

function wpe_impact_preexit(e)
	WParticleEffectVisible(wpeimpact[e].effectid,0)
	WParticleEffectAction(wpeimpact[e].effectid,3)
end