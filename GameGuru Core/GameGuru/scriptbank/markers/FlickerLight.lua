-- DESCRIPTION: Flickers the light as though supplied by a faulty electrical connection. Specify the [Faults=50] in the electricity supply. Use the [LightOn!=1] state to decide if the light is initially off or on.

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_lightflicker = {}

local nTime = 0

function FlickerLight_properties(e, faults, lighton)
 module_lightcontrol.init(e, lighton)
 g_lightflicker[e]['faults'] = faults
end 

function FlickerLight_init(e)
 g_lightflicker[e] = {}
 g_lightflicker[e]['faults'] = 50
 lightNum = GetEntityLightNumber( e )
 g_lightflicker[e]['range'] = GetLightRange ( lightNum )
end

function FlickerLight_main(e)
 if module_lightcontrol.control(e) == 1 then
  lightNum = GetEntityLightNumber( e )
  local nRandom = math.random(0,1000)
  if nRandom > g_lightflicker[e]['faults'] then
   SetLightRange(lightNum,g_lightflicker[e]['range'])
  else
   SetLightRange(lightNum,0)
  end
 end
end
