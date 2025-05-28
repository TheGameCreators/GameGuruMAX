-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Wicked Particle Emmitter - Blast v3
-- DESCRIPTION: Attach to an object and activate by a linked switch or zone
-- DESCRIPTION: [WPEFILE$="particlesbank//wpe//dirt-splash4.pe"]
-- DESCRIPTION: [IsActive!=0]
-- DESCRIPTION: <Sound0> Explosion/Impact Effect

local wpeblast 		= {}
local wpefile		= {}
local isactive		= {}
local effectid		= {}
local blasttimer	= {}
local status		= {}

function wpe_blast_properties(e, wpefile,isactive)
	wpeblast[e].wpefile = wpefile
	wpeblast[e].effectid = WParticleEffectLoad(wpefile)
	wpeblast[e].isactive = isactive	or 0
end

function wpe_blast_init(e)
	wpeblast[e] = {}
	wpeblast[e].isactive = 0	
	wpeblast[e].effectid = ""
	wpeblast[e].blasttimer = math.floor(math.random(-1.5,0))
	wpeblast[e].hurtplayer = -1
	
	status[e] = "init"
end

function wpe_blast_main(e)
	if status[e] == "init" then
		if wpeblast[e].isactive == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end
	if g_Entity[e]['activated'] == 1 then
		wpeblast[e].blasttimer = wpeblast[e].blasttimer + GetElapsedTime()
		if wpeblast[e].blasttimer > 2.5 then
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
				WParticleEffectPosition(wpeblast[e].effectid,posx,theight,posz)
				WParticleEffectAction(wpeblast[e].effectid,1) -- WParticleEffectAction(effectid,Action) - Action =  1=Burst all 2=Pause 3=Resume 4=Restart
				if randdistx >= -50 and randdistx <= 50 and randdistz >= -50 and randdistz <= 50 then
					wpeblast[e].hurtplayer = 0
				end
				wpeblast[e].blasttimer = math.floor(math.random(-1.5,0))
			end
		end
		if wpeblast[e].hurtplayer >= 0 then wpeblast[e].hurtplayer = wpeblast[e].hurtplayer + GetElapsedTime() end
		if wpeblast[e].hurtplayer > 0.950 then --Hurt player after 950 miliseconds.
			HurtPlayer(-1,50)
			wpeblast[e].hurtplayer = -1
		end
	end
	if g_Entity[e]['health'] <= 0 then
		SetPreExitValue(e,1)
	end	
end

function wpe_blast_preexit(e)
	WParticleEffectVisible(wpeblast[e].effectid,0)
	WParticleEffectAction(wpeblast[e].effectid,3)
end	
