-- DESCRIPTION: Character will patrol using flags that are nearest or connected, and then play [SPEECH1$=""].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_patrol_and_speak = {}
g_patrol_and_speak_behavior = {}
g_patrol_and_speak_behavior_count = 0

function patrol_and_speak_init_file(e,scriptfile)
 g_patrol_and_speak[e] = {}
 g_patrol_and_speak[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_patrol_and_speak_behavior_count = master_interpreter_core.masterinterpreter_load (g_patrol_and_speak[e], g_patrol_and_speak_behavior )
 patrol_and_speak_properties(e,"")
end

function patrol_and_speak_properties(e, speech1)
 g_patrol_and_speak[e]['speech1'] = speech1
 master_interpreter_core.masterinterpreter_restart (g_patrol_and_speak[e], g_Entity[e])
end

function patrol_and_speak_main(e)
 if g_patrol_and_speak[e] ~= nil and g_patrol_and_speak_behavior_count > 0 then
  g_patrol_and_speak_behavior_count = master_interpreter_core.masterinterpreter (g_patrol_and_speak_behavior, g_patrol_and_speak_behavior_count, e, g_patrol_and_speak[e], g_Entity[e])
 end
end

