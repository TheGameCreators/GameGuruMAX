-- V2 by Lee
-- DESCRIPTION: This will control a character using <Soldier Animations> to engage their enemy within view using a prone Animations
-- DESCRIPTION: Is capable of attacking their target using a <Shooting Weapon>
-- DESCRIPTION: Can [!CanReposition=1] when in danger
-- DESCRIPTION: Can [AdvanceToDistance=600] when want to get closer
-- DESCRIPTION: Can [ShootAtDistance=400] when want to take a shot
-- DESCRIPTION: Initially [!InProne=0] state
master_interpreter_core = require "scriptbank\\masterinterpreter"

g_prone_attack = {}
g_prone_attack_behavior = {}
g_prone_attack_behavior_count = 0

function prone_attack_init_file(e,scriptfile)
 g_prone_attack[e] = {}
 g_prone_attack[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_prone_attack_behavior_count = master_interpreter_core.masterinterpreter_load (g_prone_attack[e], g_prone_attack_behavior )
 prone_attack_properties(e,1)
end

function prone_attack_properties(e, canreposition, advancetodistance, shootatdistance, inprone)
 g_prone_attack[e]['canreposition'] = canreposition
 g_prone_attack[e]['advancetodistance'] = advancetodistance
 g_prone_attack[e]['shootatdistance'] = shootatdistance
 g_prone_attack[e]['inprone'] = inprone
 master_interpreter_core.masterinterpreter_restart (g_prone_attack[e], g_Entity[e])
end

function prone_attack_main(e)
 if g_prone_attack[e] ~= nil and g_prone_attack_behavior_count > 0 then
  local returnvalue = master_interpreter_core.masterinterpreter (g_prone_attack_behavior, g_prone_attack_behavior_count, e, g_prone_attack[e], g_Entity[e])
  if returnvalue >= 0 then
   g_prone_attack_behavior_count = returnvalue
  else
   local rapiddamageprocessingcount = math.abs(returnvalue)
   for rapiddamageprocessing = 1, rapiddamageprocessingcount do
    local returnvalue = master_interpreter_core.masterinterpreter (g_prone_attack_behavior, g_prone_attack_behavior_count, e, g_prone_attack[e], g_Entity[e])
    if returnvalue >= 0 then
     g_prone_attack_behavior_count = returnvalue
	end
   end
  end
 end
end
