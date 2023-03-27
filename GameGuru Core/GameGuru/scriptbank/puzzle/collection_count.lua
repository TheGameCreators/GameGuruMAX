-- Collelction Count v2
-- DESCRIPTION: Collect all [OBJECTIVES=6] items to win.
-- DESCRIPTION: Set the [RANGE=100(50,3000)] distance
-- DESCRIPTION: Displays hows how many objectives have been completed for [DURATION=1000] milliseconds.
-- DESCRIPTION: Plays <Sound0> when completed
-- DESCRIPTION: Select [@COMPLETION=1(1=End Level, 2=Activate if Used)] controls whether to end level or activate if used object.

g_collection_count = {}
g_collection_count_counts = 0

local collection_count_active = {}

function collection_count_properties(e, objectives, range, duration, completion)
	g_collection_count[e]['objectives'] = objectives
	g_collection_count[e]['range'] = range
	g_collection_count[e]['duration'] = promptduration
	g_collection_count[e]['completion'] = completion
end 

function collection_count_init(e)
	g_collection_count[e] = {}
	g_collection_count[e]['objectives'] = 6
	g_collection_count[e]['range'] = 70
	g_collection_count[e]['duration'] = 1000
	g_collection_count[e]['completion'] = 1
	collection_count_active[e] = 1
end
	 
function collection_count_main(e)
	if collection_count_active[e] == nil then
		collection_count_active[e] = 1
	end
	if collection_count_active[e] == 1 then
		local PlayerDist = GetPlayerDistance(e)
		local nrange = g_collection_count[e]['range']
		if PlayerDist <= nrange then 		
			local npduration = g_collection_count[e]['promptduration']
			g_collection_count_counts = g_collection_count_counts + 1
			PlaySound(e,0)
			PerformLogicConnections(e)
			Destroy(e)
			collection_count_active[e] = 0
			if g_collection_count_counts >= g_collection_count[e]['objectives']	then 
				if g_collection_count[e]['completion'] == 1 then JumpToLevelIfUsed(e) end
				if g_collection_count[e]['completion'] == 2 then ActivateIfUsed(e) end
			else 
				PromptDuration(g_collection_count_counts.."/"..g_collection_count[e]['objectives']	.." complete", npduration)
			end 
		end
	end
end
