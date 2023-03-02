-- DESCRIPTION: When the player is within [RANGE=400] distance, this will walk away from the player. The character will stop walking away when it's [STOPDISTANCE=800] away from the player.

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_walk_away = {}
g_walk_away_behavior = {}
g_walk_away_behavior_count = 0

function walk_away_init(e)
 g_walk_away[e] = {}
 g_walk_away[e]["bycfilename"] = "scriptbank\\people\\walk_away.byc"
 g_walk_away_behavior_count = master_interpreter_core.masterinterpreter_load (g_walk_away[e], g_walk_away_behavior )
 walk_away_properties(e,400,800)
end

function walk_away_properties(e, range, stopdistance)
 g_walk_away[e]['range'] = range
 g_walk_away[e]['stopdistance'] = stopdistance
 master_interpreter_core.masterinterpreter_restart (g_walk_away[e], g_Entity[e])
end

function walk_away_main(e)
 if g_walk_away[e] ~= nil and g_walk_away_behavior_count > 0 then
  g_walk_away_behavior_count = master_interpreter_core.masterinterpreter (g_walk_away_behavior, g_walk_away_behavior_count, e, g_walk_away[e], g_Entity[e])
 end
end

