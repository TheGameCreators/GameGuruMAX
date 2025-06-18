-- DESCRIPTION: This will control a character using <Soldier Animations> to engage their enemy within view using cover Animations
-- DESCRIPTION: Is capable of attacking their target using a <Shooting Weapon>
-- DESCRIPTION: Can [BreakCoverDistance=300] and move to best cover zone
-- DESCRIPTION: Initially [!AwareOfTarget=0] for quicker response
master_interpreter_core = require "scriptbank\\masterinterpreter"

g_cover_attack = {}
g_cover_attack_behavior = {}
g_cover_attack_behavior_count = 0

function cover_attack_init_file(e,scriptfile)
 g_cover_attack[e] = {}
 g_cover_attack[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_cover_attack_behavior_count = master_interpreter_core.masterinterpreter_load (g_cover_attack[e], g_cover_attack_behavior )
 cover_attack_properties(e,1)
end

function cover_attack_properties(e, breakcoverdistance, awareoftarget)
 g_cover_attack[e]['breakcoverdistance'] = breakcoverdistance
 g_cover_attack[e]['awareoftarget'] = awareoftarget
 master_interpreter_core.masterinterpreter_restart (g_cover_attack[e], g_Entity[e])
end

function cover_attack_main(e)
 if g_cover_attack[e] ~= nil and g_cover_attack_behavior_count > 0 then
  local returnvalue = master_interpreter_core.masterinterpreter (g_cover_attack_behavior, g_cover_attack_behavior_count, e, g_cover_attack[e], g_Entity[e])
  if returnvalue >= 0 then
   g_cover_attack_behavior_count = returnvalue
  else
   local rapiddamageprocessingcount = math.abs(returnvalue)
   for rapiddamageprocessing = 1, rapiddamageprocessingcount do
    local returnvalue = master_interpreter_core.masterinterpreter (g_cover_attack_behavior, g_cover_attack_behavior_count, e, g_cover_attack[e], g_Entity[e])
    if returnvalue >= 0 then
     g_cover_attack_behavior_count = returnvalue
	end
   end
  end
 end
end
