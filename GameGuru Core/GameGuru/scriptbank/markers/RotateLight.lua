-- DESCRIPTION: Rotates the light around the Y axis at a specified [Speed=50]. You can also specify whether the rotation is [AntiClockwise!=0]. Use the [LightOn!=1] state to decide if the light is initially off or on.

module_lightcontrol = require "scriptbank\\markers\\module_lightcontrol"

g_lightrotate = {}

function RotateLight_properties(e, rotationspeed, anticlockwise, lighton)
 module_lightcontrol.init(e, lighton)
 g_lightrotate[e]['angle'] = 0
 g_lightrotate[e]['speed'] = rotationspeed
 g_lightrotate[e]['anticlockwise'] = anticlockwise
 local lightNum = GetEntityLightNumber( e )
 x, y, z = GetLightAngle(lightNum)
 g_lightrotate[e]['angle'] = y
end 

function RotateLight_init(e)
   g_lightrotate[e] = {}
   g_lightrotate[e]['angle'] = 0
   g_lightrotate[e]['speed'] = 50
   g_lightrotate[e]['anticlockwise'] = 0
   local lightNum = GetEntityLightNumber( e )
   x, y, z = GetLightAngle(lightNum)
   g_lightrotate[e]['angle'] = y
end

function RotateLight_main(e)
 if module_lightcontrol.control(e) == 1 then
  if g_lightrotate[e]['anticlockwise'] == 0 then
   g_lightrotate[e]['angle']=g_lightrotate[e]['angle']+(g_lightrotate[e]['speed']/3600.0)
  else
   g_lightrotate[e]['angle']=g_lightrotate[e]['angle']-(g_lightrotate[e]['speed']/3600.0)
  end
  if g_lightrotate[e]['angle'] > 1.0 then g_lightrotate[e]['angle'] = g_lightrotate[e]['angle'] - 2.0 end
  if g_lightrotate[e]['angle'] < -1.0 then g_lightrotate[e]['angle'] = g_lightrotate[e]['angle'] + 2.0 end
  local lightNum = GetEntityLightNumber( e )
  x, y, z = GetLightAngle(lightNum)
  local vectory = g_lightrotate[e]['angle']
  SetLightAngle(lightNum,x,vectory,z)
 end
end
