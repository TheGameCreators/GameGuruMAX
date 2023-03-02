-- Light Control Module

g_module_lightcontrol = {}

local module_lightcontrol = {}

function module_lightcontrol.init(e,lighton)
 g_module_lightcontrol[e] = {}
 g_module_lightcontrol[e]['initialstate'] = lighton
 g_module_lightcontrol[e]['initialrange'] = GetLightRange ( GetEntityLightNumber( e ) )
end

function module_lightcontrol.control(e)
 if g_module_lightcontrol[e] ~= nil then
  -- handle initial state of light
  if g_module_lightcontrol[e]['initialstate'] == nil then g_module_lightcontrol[e]['initialstate'] = 1 end
  if g_module_lightcontrol[e]['initialstate'] >= 0 then
   if g_module_lightcontrol[e]['initialstate'] == 0 then SetActivatedWithMP(e,101) end
   if g_module_lightcontrol[e]['initialstate'] == 1 then SetActivatedWithMP(e,201) end
   g_module_lightcontrol[e]['initialstate'] = -1
  end
  -- respond to external activation
  if g_module_lightcontrol[e]['activestate'] == 0 then
   if g_Entity[e]['activated'] == 1 then
    SetActivatedWithMP(e,201)
   end
  else
   if g_Entity[e]['activated'] == 0 then
    SetActivatedWithMP(e,101)
   end
  end
  -- proximity independence
  if g_Entity[e]['activated'] == 100 then
   g_module_lightcontrol[e]['activestate'] = 0
   SetActivated(e,0)
  end
  if g_Entity[e]['activated'] == 101 then
   lightNum = GetEntityLightNumber( e )
   SetLightRange(lightNum,0)
   g_module_lightcontrol[e]['activestate'] = 0
   SetActivated(e,0)
  end
  if g_Entity[e]['activated'] == 201 then
   lightNum = GetEntityLightNumber( e )
   SetLightRange(lightNum,g_module_lightcontrol[e]['initialrange'])
   g_module_lightcontrol[e]['activestate'] = 1
   SetActivated(e,1)
  end  
  return g_module_lightcontrol[e]['activestate']
 else
  return 0
 end
end

return module_lightcontrol
