-- DESCRIPTION: Character will patrol using flags that are nearest or connected, and has options to [!AllowHeadshot].
master_interpreter_core = require "scriptbank\\masterinterpreter"

g_patrol = {}
g_patrol_behavior = {}
g_patrol_behavior_count = 0

function patrol_init(e)
 g_patrol[e] = {}
 g_patrol[e]["bycfilename"] = "scriptbank\\people\\patrol.byc"
 g_patrol_behavior_count = master_interpreter_core.masterinterpreter_load (g_patrol[e], g_patrol_behavior )
 patrol_properties(e,1)
end

function patrol_properties(e,allowheadshot)
 g_patrol[e]['allowheadshot'] = allowheadshot
 master_interpreter_core.masterinterpreter_restart (g_patrol[e], g_Entity[e])
end

function patrol_main(e)
 if g_patrol[e] ~= nil and g_patrol_behavior_count > 0 then
  g_patrol_behavior_count = master_interpreter_core.masterinterpreter (g_patrol_behavior, g_patrol_behavior_count, e, g_patrol[e], g_Entity[e])
 end
end

