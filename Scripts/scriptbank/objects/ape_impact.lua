-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Advanced_Particle Effect - impact v1
-- DESCRIPTION: Advanced Particle Effect - Impact
-- DESCRIPTION: Attach to an object set Always Active = ON
-- DESCRIPTION: [APEFILE$="particlesbank//advanced//dirt-splash4.pe"]
-- DESCRIPTION: [IsActive!=0]
-- DESCRIPTION: <Sound0> explosion.

local impact 		= {}
local apefile		= {}
local isactive		= {}
local impactid 		= {}
local impacttimer 	= {}
local hurtplayer 	= {}
local status		= {}

function ape_impact_properties(e, apefile, isactive)
	impact[e].apefile = apefile
	impact[e].impactid = WParticleEffectLoad(apefile)
	impact[e].isactive = isactive or 0	
end

function ape_impact_init(e)
	impact[e] = {}
	impact[e].apefile = ""
	impact[e].isactive = 0	
	impact[e].impactid = ""
	impact[e].impacttimer = math.floor(math.random(-1.5,0))
	impact[e].hurtplayer = -1
	
	status[e] = "init"
end

function ape_impact_main(e)

	if status[e] == "init" then
		if impact[e].isactive == 1 then SetActivated(e,1) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		impact[e].impacttimer = impact[e].impacttimer + GetElapsedTime()
		if impact[e].impacttimer > 2.5 then
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
				WParticleEffectPosition(impact[e].impactid,posx,theight,posz)
				WParticleEffectAction(impact[e].impactid,1) -- burst all
				if randdistx >= -50 and randdistx <= 50 and randdistz >= -50 and randdistz <= 50 then
					impact[e].hurtplayer = 0
				end
				impact[e].impacttimer = math.floor(math.random(-1.5,0))
			end
		end
		if impact[e].hurtplayer >= 0 then impact[e].hurtplayer = impact[e].hurtplayer + GetElapsedTime() end
		if impact[e].hurtplayer > 0.950 then --Hurt player after 950 miliseconds.
			HurtPlayer(-1,50)
			impact[e].hurtplayer = -1
		end
	end
	if g_Entity[e]['activated'] == 0 then
		WParticleEffectVisible(impact[e].impactid,0)
		WParticleEffectAction(impact[e].impactid,3)
	end		
end