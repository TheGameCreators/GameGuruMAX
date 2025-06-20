-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- FlameLight v4 by Lee and Necrym59
-- DESCRIPTION: Light will vary as though emitted from a candle or camp fire, with control over variance [Intensity=50]
-- DESCRIPTION: Use the [LightOn!=1] state to decide if the light is initially off or on.
-- DESCRIPTION: Use the [LightMove!=1] state to set if the light moves.
-- DESCRIPTION: Use the [MoveAmount=1(1,100)] to set movement amount of the light.
-- DESCRIPTION: Use the [GlowInterval=10(1,100)] to adjust glow interval of the light.

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_lightflame = {}
local move_timer = {}
local lx = {}
local ly = {}
local lz = {}
local lRandom = {}
local glowtime = {}

function FlameLight_properties(e, intensity, lighton, lightmove, moveamount, glowinterval)
	module_lightcontrol.init(e, lighton)
	g_lightflame[e]['intensity'] = intensity
	g_lightflame[e]['lightmove'] = lightmove or 1
	g_lightflame[e]['moveamount'] = moveamount
	g_lightflame[e]['glowinterval'] = glowinterval
end

function FlameLight_init(e)
	g_lightflame[e] = {}
	g_lightflame[e]['intensity'] = 50
	lightNum = GetEntityLightNumber( e )
	r, g, b = GetLightRGB ( lightNum )
	g_lightflame[e]['r'] = r
	g_lightflame[e]['g'] = g
	g_lightflame[e]['b'] = b
	g_lightflame[e]['range'] = GetLightRange(lightNum)
	g_lightflame[e]['cr'] = r
	g_lightflame[e]['cg'] = g
	g_lightflame[e]['cb'] = b
	g_lightflame[e]['crange'] = GetLightRange(lightNum)
	g_lightflame[e]['lightmove'] = 1
	g_lightflame[e]['moveamount'] = 1
	g_lightflame[e]['glowinterval'] = 10
	FlameLight_properties(e,50,1,1,1,10)
	move_timer[e] = 0
	glowtime[e] = 0
	lx[e],ly[e],lz[e] = GetLightPosition(lightNum)
	lRandom[e] = math.random(1,g_lightflame[e]['moveamount'])
	if lRandom[e] <= 0 then lRandom[e] = 1 end
end

function FlameLight_main(e)
	if module_lightcontrol.control(e) == 1 then
		lightNum = GetEntityLightNumber( e )
		if lightNum ~= nil and g_Time > glowtime[e] then
			local nRandom = math.random(0,2)
			lRandom[e] = math.random(1,g_lightflame[e]['moveamount'])
			if nRandom == 0 then
				local nVariance = math.random(0,g_lightflame[e]['intensity'])-(g_lightflame[e]['intensity']/2)
				g_lightflame[e]['cr'] = g_lightflame[e]['r'] + nVariance
				g_lightflame[e]['cg'] = g_lightflame[e]['g'] + nVariance
				g_lightflame[e]['cb'] = g_lightflame[e]['b'] + nVariance
				g_lightflame[e]['crange'] = g_lightflame[e]['range'] + nVariance
				if g_lightflame[e]['cr'] < 0 then g_lightflame[e]['cr'] = 0 end
				if g_lightflame[e]['cg'] < 0 then g_lightflame[e]['cg'] = 0 end
				if g_lightflame[e]['cb'] < 0 then g_lightflame[e]['cb'] = 0 end
				if g_lightflame[e]['crange'] < 0 then g_lightflame[e]['crange'] = 0 end
				if g_lightflame[e]['cr'] > 255 then g_lightflame[e]['cr'] = 255 end
				if g_lightflame[e]['cg'] > 255 then g_lightflame[e]['cg'] = 255 end
				if g_lightflame[e]['cb'] > 255 then g_lightflame[e]['cb'] = 255 end
			end
			r, g, b = GetLightRGB ( lightNum )
			if r < g_lightflame[e]['cr'] then r = r + 1 end
			if g < g_lightflame[e]['cg'] then g = g + 1 end
			if b < g_lightflame[e]['cb'] then b = b + 1 end
			if r > g_lightflame[e]['cr'] then r = r - 1 end
			if g > g_lightflame[e]['cg'] then g = g - 1 end
			if b > g_lightflame[e]['cb'] then b = b - 1 end
			range = GetLightRange(lightNum);
			if range < g_lightflame[e]['crange'] then range = range + 1 end
			if range > g_lightflame[e]['crange'] then range = range - 1 end
			SetLightRGB(lightNum,r,g,b)
			SetLightRange(lightNum,range)
			if g_lightflame[e]['lightmove'] == 1 and g_Time > move_timer[e] then
				SetLightPosition(lightNum,lx[e]+math.sin(lRandom[e]/500),ly[e]+math.sin(lRandom[e]/100),lz[e]+math.cos(lRandom[e]/500))
				move_timer[e] = g_Time+50
			end
			glowtime[e] = g_Time + g_lightflame[e]['glowinterval']
		end
	end
end
