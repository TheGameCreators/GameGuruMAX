-- Activation Control Module

g_module_activationcontrol = {}

local module_activationcontrol = {}

function module_activationcontrol.init(e,activated)
 g_module_activationcontrol[e] = {}
 g_module_activationcontrol[e]['initialstate'] = activated
end

function module_activationcontrol.control(e)
 -- handle initial state of activation
 if g_module_activationcontrol[e] == nil then g_module_activationcontrol[e] = {} end
 if g_module_activationcontrol[e]['initialstate'] == nil then g_module_activationcontrol[e]['initialstate'] = 1 end
 if g_module_activationcontrol[e]['initialstate'] >= 0 then
  if g_module_activationcontrol[e]['initialstate'] == 0 then SetActivatedWithMP(e,101) end
  if g_module_activationcontrol[e]['initialstate'] == 1 then SetActivatedWithMP(e,201) end
  g_module_activationcontrol[e]['initialstate'] = -1
 end
 -- respond to external activation
 if g_module_activationcontrol[e]['activestate'] == 0 then
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
  g_module_activationcontrol[e]['activestate'] = 0
  SetActivated(e,0)
 end
 if g_Entity[e]['activated'] == 101 then
  g_module_activationcontrol[e]['activestate'] = 0
  SetActivated(e,0)
 end
 if g_Entity[e]['activated'] == 201 then
  g_module_activationcontrol[e]['activestate'] = 1
  SetActivated(e,1)
 end  
 return g_module_activationcontrol[e]['activestate']
end

return module_activationcontrol
