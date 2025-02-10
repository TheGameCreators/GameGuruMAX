-- DESCRIPTION: When the player is within view range distance of this character, move towards the player, and then within [RANGE=150(50,500)] distance, play [SPEECH1$=""].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_follow_cover = {}
g_follow_cover_behavior = {}
g_follow_cover_behavior_count = 0

function follow_cover_init_file(e,scriptfile)
 g_follow_cover[e] = {}
 g_follow_cover[e]["bycfilename"] = "scriptbank\\" .. scriptfile .. ".byc"
 g_follow_cover_behavior_count = master_interpreter_core.masterinterpreter_load (g_follow_cover[e], g_follow_cover_behavior )
 follow_cover_properties(e,300,"")
end

function follow_cover_properties(e, range, speech1)
 g_follow_cover[e]['range'] = range
 g_follow_cover[e]['speech1'] = speech1
 master_interpreter_core.masterinterpreter_restart (g_follow_cover[e], g_Entity[e])
end

function follow_cover_main(e)
 if g_follow_cover[e] ~= nil and g_follow_cover_behavior_count > 0 then
  g_follow_cover_behavior_count = master_interpreter_core.masterinterpreter (g_follow_cover_behavior, g_follow_cover_behavior_count, e, g_follow_cover[e], g_Entity[e])
 end
end

