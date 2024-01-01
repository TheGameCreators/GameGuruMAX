-- DESCRIPTION: The <Unarmed> character is a bound hostage. Reach the character to unbound them, and then they will run away and stop when they reach a [StopDistance=800]. They will use <Sound0> when hurt, and <Sound1> when they are freed. They can optionally be [Injured!=0].

master_interpreter_core = require "scriptbank\\masterinterpreter"

g_hostage_runs_away = {}
g_hostage_runs_away_behavior = {}
g_hostage_runs_away_behavior_count = 0

function hostage_runs_away_init(e)
 g_hostage_runs_away[e] = {}
 g_hostage_runs_away[e]["bycfilename"] = "scriptbank\\people\\hostage_runs_away.byc"
 g_hostage_runs_away_behavior_count = master_interpreter_core.masterinterpreter_load (g_hostage_runs_away[e], g_hostage_runs_away_behavior )
 hostage_runs_away_properties(e,800,0)
end

function hostage_runs_away_properties(e, stopdistance, injured)
 g_hostage_runs_away[e]['stopdistance'] = stopdistance
 g_hostage_runs_away[e]['injured'] = injured
 master_interpreter_core.masterinterpreter_restart (g_hostage_runs_away[e], g_Entity[e])
end

function hostage_runs_away_main(e)
 if g_hostage_runs_away[e] ~= nil and g_hostage_runs_away_behavior_count > 0 then
  g_hostage_runs_away_behavior_count = master_interpreter_core.masterinterpreter (g_hostage_runs_away_behavior, g_hostage_runs_away_behavior_count, e, g_hostage_runs_away[e], g_Entity[e])
 end
end
