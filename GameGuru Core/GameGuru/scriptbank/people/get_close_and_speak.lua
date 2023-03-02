-- DESCRIPTION: When the player is within view range distance of this character, move towards the player, and then within [RANGE=150(50,500)] distance, play [SPEECH1$=""].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_get_close_and_speak = {}
g_get_close_and_speak_behavior = {}
g_get_close_and_speak_behavior_count = 0

function get_close_and_speak_init(e)
 g_get_close_and_speak[e] = {}
 g_get_close_and_speak[e]["bycfilename"] = "scriptbank\\people\\get_close_and_speak.byc"
 g_get_close_and_speak_behavior_count = master_interpreter_core.masterinterpreter_load (g_get_close_and_speak[e], g_get_close_and_speak_behavior )
 get_close_and_speak_properties(e,300,"")
end

function get_close_and_speak_properties(e, range, speech1)
 g_get_close_and_speak[e]['range'] = range
 g_get_close_and_speak[e]['speech1'] = speech1
 master_interpreter_core.masterinterpreter_restart (g_get_close_and_speak[e], g_Entity[e])
end

function get_close_and_speak_main(e)
 if g_get_close_and_speak[e] ~= nil and g_get_close_and_speak_behavior_count > 0 then
  g_get_close_and_speak_behavior_count = master_interpreter_core.masterinterpreter (g_get_close_and_speak_behavior, g_get_close_and_speak_behavior_count, e, g_get_close_and_speak[e], g_Entity[e])
 end
end

