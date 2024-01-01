-- DESCRIPTION: When player is within [RANGE=200] distance, character will face the player and play [SPEECH1$=""].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_stand_and_speak = {}
g_stand_and_speak_behavior = {}
g_stand_and_speak_behavior_count = 0

function stand_and_speak_init_file(e,scriptfile)
 g_stand_and_speak[e] = {}
 g_stand_and_speak[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_stand_and_speak_behavior_count = master_interpreter_core.masterinterpreter_load (g_stand_and_speak[e], g_stand_and_speak_behavior )
 stand_and_speak_properties(e,300,"")
end

function stand_and_speak_properties(e, range, speech1)
 g_stand_and_speak[e]['range'] = range
 g_stand_and_speak[e]['speech1'] = speech1
 master_interpreter_core.masterinterpreter_restart (g_stand_and_speak[e], g_Entity[e])
end

function stand_and_speak_main(e)
 if g_stand_and_speak[e] ~= nil and g_stand_and_speak_behavior_count > 0 then
  g_stand_and_speak_behavior_count = master_interpreter_core.masterinterpreter (g_stand_and_speak_behavior, g_stand_and_speak_behavior_count, e, g_stand_and_speak[e], g_Entity[e])
 end
end

