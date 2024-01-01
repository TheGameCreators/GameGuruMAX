-- DESCRIPTION: Light will vary as though emitted from a candle or camp fire, with control over variance [Intensity=50]. Use the [LightOn!=1] state to decide if the light is initially off or on.

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_lightflame = {}

function FlameLight_properties(e, intensity, lighton)
 module_lightcontrol.init(e, lighton)
 g_lightflame[e]['intensity'] = intensity
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
	FlameLight_properties(e, 50, 1)
end

function FlameLight_main(e)
 if module_lightcontrol.control(e) == 1 then
	lightNum = GetEntityLightNumber( e )
	if lightNum ~= nil then
	local nRandom = math.random(0,2)
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
	end
 end
end
