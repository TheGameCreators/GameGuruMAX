-- DESCRIPTION: When the player is within [RANGE=400] distance, the character will start wandering about. A RANGE of zero will cause the character to start wandering from the moment the game starts.

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_wander_around = {}
g_wander_around_behavior = {}
g_wander_around_behavior_count = 0

function wander_around_init_file(e,scriptfile)
 g_wander_around[e] = {}
 g_wander_around[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_wander_around_behavior_count = master_interpreter_core.masterinterpreter_load (g_wander_around[e], g_wander_around_behavior )
 wander_around_properties(e,300)
end

function wander_around_properties(e, range)
 g_wander_around[e]['range'] = range
 master_interpreter_core.masterinterpreter_restart (g_wander_around[e], g_Entity[e])
end

function wander_around_main(e)
 if g_wander_around[e] ~= nil and g_wander_around_behavior_count > 0 then
  g_wander_around_behavior_count = master_interpreter_core.masterinterpreter (g_wander_around_behavior, g_wander_around_behavior_count, e, g_wander_around[e], g_Entity[e])
 end
end

