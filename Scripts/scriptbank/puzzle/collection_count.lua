-- Collection Count v7 by Necrym59
-- DESCRIPTION: This behavior allows for a Collection Item for pickup
-- DESCRIPTION: Set a Collection Control behavior for a collection configuration
-- DESCRIPTION: [PICKUP_RANGE=100(0,100)]
-- DESCRIPTION: [TIME_BONUS=0(0,100)] in seconds
-- DESCRIPTION: [TIME_PENALTY=0(0,100)] in seconds
-- DESCRIPTION: <Sound0> plays when objective picked up

g_collection_objectives	= {}
g_collection_time		= {}
g_collection_counted	= 0

local cc_count			= {}
local pickup_range		= {}
local time_bonus		= {}
local time_penalty		= {}
local status			= {}

function collection_count_properties(e, pickup_range, time_bonus, time_penalty)
	cc_count[e].pickup_range = pickup_range
	cc_count[e].time_bonus = time_bonus or 0
	cc_count[e].time_penalty = time_penalty or 0
end

function collection_count_init(e)
	cc_count[e] = {}
	cc_count[e].pickup_range = 100
	cc_count[e].time_bonus = 0
	cc_count[e].time_penalty = 0
	status[e] = "init"
end

function collection_count_main(e)
	if status[e] == "init" then
		g_collection_counted = 0
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist <= cc_count[e].pickup_range then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if cc_count[e].time_bonus > 0 then g_collection_time = g_collection_time + (cc_count[e].time_bonus * 1000) end
		if cc_count[e].time_penalty > 0 then g_collection_time = g_collection_time - (cc_count[e].time_penalty * 1000) end
		if g_collection_counted ~= g_collection_objectives then
			g_collection_counted = g_collection_counted + 1
		end
		Destroy(e)
	end
end
