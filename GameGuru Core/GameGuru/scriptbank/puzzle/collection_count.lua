-- DESCRIPTION: Collect all [OBJECTIVES=6] items to win. Plays <Sound0> and complete the objective when player moves within [RANGE=100(50,3000)] distance of it and shows how many objectives have been completed for [DURATION=1000] milliseconds.

g_collection_count = {}
g_collection_count_counts = 0

local collection_count_active = {}

function collection_count_properties(e, objectives, range, duration)
	g_collection_count[e]['objectives'] = objectives
	g_collection_count[e]['range'] = range
	g_collection_count[e]['duration'] = promptduration
end 

function collection_count_init(e)
	g_collection_count[e] = {}
	g_collection_count[e]['objectives'] = 6
	g_collection_count[e]['range'] = 70
	g_collection_count[e]['duration'] = 1000
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
			if g_collection_count_counts >= g_collection_count[e]['objectives']	 then 
				JumpToLevelIfUsed(e)
			else 
				PromptDuration(g_collection_count_counts.."/"..g_collection_count[e]['objectives']	.." complete", npduration)
			end 
		end
	end
end
