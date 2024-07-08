-- Particle Effect Control v1
-- Can be assigned via Developer Settings to a particle and adjust in real-time the particle chosen
-- Current settings suitable for the FOUNTAIN_DIRECTION Particle Effect

g_particleeffect = {}

function particleeffect_init_name(e,name)
	g_particleeffect[e] = {}	
	g_particleeffect[e].name = name
	g_particleeffect[e].particlemovemode = 2
	g_particleeffect[e].particlerotatemode = 2
	g_particleeffect[e].status = 0
	g_particleeffect[e].ybase = g_Entity[e]['y']
	g_particleeffect[e].ycount = 0
	StartTimer(e)
end

function particleeffect_main(e)
	-- emitter activation
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < 300 then
		if g_particleeffect[e].status == 0 then
			g_particleeffect[e].status = 1
			EffectStart(e)
		end
	else
		if g_particleeffect[e].status == 1 then
			g_particleeffect[e].status = 0
			EffectStop(e)
		end
	end
	-- effect/emitter movement
	local spanyrange = 20
	g_particleeffect[e].ycount=g_particleeffect[e].ycount+0.01
	local ymove = math.cos(g_particleeffect[e].ycount)*spanyrange
	if g_particleeffect[e].particlemovemode == 1 then
		-- this moves the ENTIRE Effect
		SetPosition(e,g_Entity[e]['x'],g_particleeffect[e].ybase+spanyrange+ymove,g_Entity[e]['z'])
	end
	if g_particleeffect[e].particlemovemode == 2 then
		-- this moves ONLY the emitter position
		EffectSetLocalPosition(e,0,spanyrange+ymove,0)
	end
	-- effect/emitter rotation
	local yrotate = g_particleeffect[e].ycount*60*2
	if g_particleeffect[e].particlerotatemode == 1 then
		-- this rotates the ENTIRE Effect
		SetRotation(e,g_Entity[e]['anglex'],yrotate,g_Entity[e]['anglez'])
	end
	if g_particleeffect[e].particlerotatemode == 2 then
		-- this rotates ONLY the emitter
		EffectSetLocalRotation(e,0,yrotate,0)
	end
	-- change effect speed and opacity and color
	EffectSetSpeed(e,600.0)	
	EffectSetOpacity(e,15.0)	
	EffectSetColor(e,0,255,128)
	-- change effect particle size
	EffectSetParticleSize(e,400.0)
	-- when very close, switch to fire bursts and shoot once per second
	if PlayerDist < 150 then
		EffectSetBurstMode(e,1)
		if GetTimer(e) > 1000 then
			EffectFireBurst(e)
			StartTimer(e)
		end
	else
		EffectSetBurstMode(e,0)
	end
	-- change the bounciness when hit a floor reflection zone
	EffectSetBounciness(e,150.0)
	EffectSetFloorReflection(e,1,35)
	-- change the lifespan of the particle emitted
	EffectSetLifespan(e,50)
end
