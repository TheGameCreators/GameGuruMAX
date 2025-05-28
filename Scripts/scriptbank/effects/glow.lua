-- v3
-- DESCRIPTION: This object will vary the strength of any emissive texture applied to it between [MINRANGE=10(0,100)] and [MAXRANGE=90(0,100)],
-- DESCRIPTION: at a specific [SPEEDOFCHANGE=10(1,100)], as though glowing slightly.
-- DESCRIPTION: [ActiveAtStart!=1] if unchecked use a switch or zone trigger to activate.
-- DESCRIPTION: <Sound0> Looping sound

g_glow = {}

local ActivateAtStart	= {}
local status			= {}
local sound_volume 		= {}

function glow_properties(e, minrange, maxrange, speedofchange, ActivateAtStart)
	g_glow[e]['minrange'] = minrange
	g_glow[e]['maxrange'] = maxrange
	g_glow[e]['speedofchange'] = speedofchange
	g_glow[e]['ActivateAtStart'] = ActivateAtStart
end

function glow_init(e)
	g_glow[e] = {}
	g_glow[e]['minrange'] = 10
	g_glow[e]['maxrange'] = 90
	g_glow[e]['speedofchange'] = 10
	g_glow[e]['direction'] = 1
	g_glow[e]['value'] = 50
	g_glow[e]['ActivateAtStart'] = 1
	status[e] = "init"
	sound_volume[e] = 0
end

function glow_main(e)
	if status[e] == "init" then
		if g_glow[e]['ActivateAtStart'] == 1 then SetActivated(e,1) end
		if g_glow[e]['ActivateAtStart'] == 0 then SetActivated(e,0) end	
		sound_volume[e] = 0
		SetSoundVolume(sound_volume[e])
		status[e] = "end"
	end

	if g_Entity[e]['activated'] == 1 then
		if GetPlayerDistance(e) < 3000 then
			sound_volume[e] = (3000-GetPlayerDistance(e))/30
			LoopSound(e,0)
			SetSoundVolume(sound_volume[e])
		else
			SetSoundVolume(0)
		end	
		if math.random(0,5) == 1 then g_glow[e]['direction'] = math.random(1,2) end
		local value = g_glow[e]['value']
		if g_glow[e]['direction'] == 1 then
			value = value - g_glow[e]['speedofchange'] / 10.0
			if value < g_glow[e]['minrange'] then value = g_glow[e]['minrange'] end
		else
			value = value + g_glow[e]['speedofchange'] / 10.0
			if value > g_glow[e]['maxrange'] then value = g_glow[e]['maxrange'] end
		end
		g_glow[e]['value'] = value
		SetEntityEmissiveStrength(e,value)
	end
end
