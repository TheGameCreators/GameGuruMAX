-- DESCRIPTION: This object will vary the strength of any emissive texture applied to it between [MINRANGE=10(0,100)] and [MAXRANGE=90(0,100)], at a specific [SPEEDOFCHANGE=10(1,100)], as though glowing slightly.

g_glow = {}

function glow_properties(e, minrange, maxrange, speedofchange)
 g_glow[e]['minrange'] = minrange
 g_glow[e]['maxrange'] = maxrange
 g_glow[e]['speedofchange'] = speedofchange
end 

function glow_init(e)
 g_glow[e] = {}
 g_glow[e]['minrange'] = 10
 g_glow[e]['maxrange'] = 90
 g_glow[e]['speedofchange'] = 10
 g_glow[e]['direction'] = 1
 g_glow[e]['value'] = 50
end

function glow_main(e)
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
