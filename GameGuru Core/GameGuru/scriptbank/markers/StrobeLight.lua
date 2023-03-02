-- DESCRIPTION: Light strobe will bounce from [MinRange=1] to [MaxRange=200] at a specified [Speed=100]. Use the [LightOn!=1] state to decide if the light is initially off or on.

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_lightstrobe = {}

function StrobeLight_properties(e, minrange, maxrange,speed,frames, lighton)
 module_lightcontrol.init(e, lighton)
 g_lightstrobe[e]['minrange'] = minrange
 g_lightstrobe[e]['maxrange'] = maxrange
 g_lightstrobe[e]['speed'] = speed
end 

function StrobeLight_init(e)
	g_lightstrobe[e] = {}
	g_lightstrobe[e]['minrange'] = 1
	g_lightstrobe[e]['maxrange'] = 200
	g_lightstrobe[e]['speed'] = 100
	g_lightstrobe[e]['crange'] = 0
	g_lightstrobe[e]['cdirection'] = 0
end

function StrobeLight_main(e)
 if module_lightcontrol.control(e) == 1 then
	lightNum = GetEntityLightNumber( e )
	local nMinRange = g_lightstrobe[e]['minrange']
	local nMaxRange = g_lightstrobe[e]['maxrange']
	if g_lightstrobe[e]['cdirection'] == 0 then
	 g_lightstrobe[e]['crange'] = g_lightstrobe[e]['crange'] + (g_lightstrobe[e]['speed']/10.0)
	 if g_lightstrobe[e]['crange'] > g_lightstrobe[e]['maxrange'] then
	  g_lightstrobe[e]['crange'] = g_lightstrobe[e]['maxrange']
	  g_lightstrobe[e]['cdirection'] = 1
	 end
	else
	 g_lightstrobe[e]['crange'] = g_lightstrobe[e]['crange'] - (g_lightstrobe[e]['speed']/10.0)
	 if g_lightstrobe[e]['crange'] < g_lightstrobe[e]['minrange'] then
	  g_lightstrobe[e]['crange'] = g_lightstrobe[e]['minrange']
	  g_lightstrobe[e]['cdirection'] = 0
	 end
    end	
    SetLightRange(lightNum,g_lightstrobe[e]['crange'])
 end
end
