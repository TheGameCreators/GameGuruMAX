-- DESCRIPTION: When the character is hurt, run away and stop when they reach a [STOPDISTANCE=800]. They will use <Sound0> when hurt.

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_run_away_if_hurt = {}
g_run_away_if_hurt_behavior = {}
g_run_away_if_hurt_behavior_count = 0

function run_away_if_hurt_init(e)
 g_run_away_if_hurt[e] = {}
 g_run_away_if_hurt[e]["bycfilename"] = "scriptbank\\people\\run_away_if_hurt.byc"
 g_run_away_if_hurt_behavior_count = master_interpreter_core.masterinterpreter_load (g_run_away_if_hurt[e], g_run_away_if_hurt_behavior )
 run_away_if_hurt_properties(e,800)
end

function run_away_if_hurt_properties(e, stopdistance)
 g_run_away_if_hurt[e]['stopdistance'] = stopdistance
 master_interpreter_core.masterinterpreter_restart (g_run_away_if_hurt[e], g_Entity[e])
end

function run_away_if_hurt_main(e)
 if g_run_away_if_hurt[e] ~= nil and g_run_away_if_hurt_behavior_count > 0 then
  g_run_away_if_hurt_behavior_count = master_interpreter_core.masterinterpreter (g_run_away_if_hurt_behavior, g_run_away_if_hurt_behavior_count, e, g_run_away_if_hurt[e], g_Entity[e])
 end
end
