-- DESCRIPTION: When the target is within [RANGE=500] distance, the character will attack with <Melee Animations> the target using <Melee Weapon>, either using fists, or with a melee weapon if equipped. They will use <Sound0> and <Sound1> when attacking, and <Sound2> and <Sound3> when hurt. They can optionally [RunAway!=0] and be [InstantlyKillable!=1]. Optionally [!StarterAnimation=0] using [@StartAnimation=1(0=AnimSetList)]. Can also [!PatrolAtStart=0].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_melee_attack = {}
g_melee_attack_behavior = {}
g_melee_attack_behavior_count = 0

function melee_attack_init_file(e,scriptfile)
 g_melee_attack[e] = {}
 g_melee_attack[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_melee_attack_behavior_count = master_interpreter_core.masterinterpreter_load (g_melee_attack[e], g_melee_attack_behavior )
 melee_attack_properties(e,500,0,1,0,0,0)
end

function melee_attack_properties(e, range, runaway, instantlykillable, starteranimation, startanimation, patrolatstart)
 g_melee_attack[e]['range'] = range
 g_melee_attack[e]['runaway'] = runaway
 g_melee_attack[e]['instantlykillable'] = instantlykillable
 g_melee_attack[e]['starteranimation'] = starteranimation
 g_melee_attack[e]['startanimation'] = startanimation
 g_melee_attack[e]['patrolatstart'] = patrolatstart
 master_interpreter_core.masterinterpreter_restart (g_melee_attack[e], g_Entity[e])
end

function melee_attack_main(e)
 if g_melee_attack[e] ~= nil and g_melee_attack_behavior_count > 0 then
  local returnvalue = master_interpreter_core.masterinterpreter (g_melee_attack_behavior, g_melee_attack_behavior_count, e, g_melee_attack[e], g_Entity[e])
  if returnvalue >= 0 then
   g_melee_attack_behavior_count = returnvalue
  else
   local rapiddamageprocessingcount = math.abs(returnvalue)
   for rapiddamageprocessing = 1, rapiddamageprocessingcount do
    local returnvalue = master_interpreter_core.masterinterpreter (g_melee_attack_behavior, g_melee_attack_behavior_count, e, g_melee_attack[e], g_Entity[e])
    if returnvalue >= 0 then
     g_melee_attack_behavior_count = returnvalue
	end
   end
  end
 end
end

