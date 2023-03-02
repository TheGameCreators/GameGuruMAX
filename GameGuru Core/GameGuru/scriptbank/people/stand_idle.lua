-- DESCRIPTION: The character remains idle, standing on the spot.

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_stand_idle = {}
g_stand_idle_behavior = {}
g_stand_idle_behavior_count = 0

function stand_idle_init(e)
 g_stand_idle[e] = {}
 g_stand_idle[e]["bycfilename"] = "scriptbank\\people\\stand_idle.byc"
 g_stand_idle_behavior_count = master_interpreter_core.masterinterpreter_load (g_stand_idle[e], g_stand_idle_behavior )
 master_interpreter_core.masterinterpreter_restart (g_stand_idle[e], g_Entity[e])
end

function stand_idle_main(e)
 if g_stand_idle[e] ~= nil and g_stand_idle_behavior_count > 0 then
  g_stand_idle_behavior_count = master_interpreter_core.masterinterpreter (g_stand_idle_behavior, g_stand_idle_behavior_count, e, g_stand_idle[e], g_Entity[e])
 end
end

