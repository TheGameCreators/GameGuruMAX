-- Collection Count v5 by Necrym59 and Lee
-- DESCRIPTION: Allows for a collection of items to complete the level or activate an IfUsed entity.
-- DESCRIPTION: Collect all [OBJECTIVES=6] to win within the allotted
-- DESCRIPTION: [COLLECTION_TIME=60(10,500)] in seconds
-- DESCRIPTION: [PICKUP_RANGE=80(0,100)]
-- DESCRIPTION: [TIME_BONUS=0(0,100)] in seconds
-- DESCRIPTION: [TIME_PENALTY=0(0,100)] in seconds
-- DESCRIPTION: [HEALTH_BONUS=0(0,100)] in units
-- DESCRIPTION: [HEALTH_PENALTY=0(0,100)] in units
-- DESCRIPTION: [@COMPLETION=1(1=End Level, 2=Activate if Used)] controls whether to end level or activate if used object.
-- DESCRIPTION: <Sound0> plays when objective picked up
-- DESCRIPTION: <Sound1> plays when collection completed

g_collection_count = {}
g_collection_count_counts = 0
g_collection_count_time = {}

local collection_time = {}
local pickup_range = {}
local time_bonus = {}
local time_penalty = {}
local health_bonus = {}
local health_penalty = {}
local completion = {}
local status = {}

function collection_count_properties(e, objectives, collection_time, pickup_range, time_bonus, time_penalty, health_bonus, health_penalty, completion)
	g_collection_count[e].objectives = objectives
	g_collection_count[e].collection_time = collection_time
	g_collection_count[e].pickup_range = pickup_range
	g_collection_count[e].time_bonus =  time_bonus or 0
	g_collection_count[e].time_penalty = time_penalty or 0
	g_collection_count[e].health_bonus = health_bonus or 0	
	g_collection_count[e].health_penalty = health_penalty or 0
	g_collection_count[e].completion = completion
end

function collection_count_init(e)
	g_collection_count[e] = {}
	g_collection_count[e].objectives = 6
	g_collection_count[e].collection_time = 60
	g_collection_count[e].pickup_range = 80
	g_collection_count[e].time_bonus = 0
	g_collection_count[e].time_penalty = 0
	g_collection_count[e].health_bonus = 0	
	g_collection_count[e].health_penalty = 0	
	g_collection_count[e].completion = 1

	status[e] = "init"
end

function collection_count_main(e)
	if status[e] == "init" then
		g_collection_count_time = g_collection_count[e].collection_time * 1000
		g_collection_count_counts = 0
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if PlayerDist <= g_collection_count[e].pickup_range then
		if g_collection_count_counts == 0 then StartTimer(e) end
		PlaySound(e,0)
		PerformLogicConnections(e)
		Destroy(e)
		g_collection_count_time = g_collection_count_time + (g_collection_count[e].time_bonus * 1000)
		g_collection_count_time = g_collection_count_time - (g_collection_count[e].time_penalty * 1000)
		if g_collection_count[e].health_bonus > 0 then
			SetPlayerHealth(g_PlayerHealth + g_collection_count[e].health_bonus)
			if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end			
			SetPlayerHealthCore(g_PlayerHealth)
		end
		if g_collection_count[e].health_penalty > 0 then
			SetPlayerHealth(g_PlayerHealth - g_collection_count[e].health_penalty)
			SetPlayerHealthCore(g_PlayerHealth)
		end
		if g_collection_count_counts ~= g_collection_count[e].objectives then
			g_collection_count_counts = g_collection_count_counts + 1
		end
		if g_collection_count_counts >= g_collection_count[e].objectives then
			PlaySound(e,1)
			if g_collection_count[e].completion == 1 then JumpToLevelIfUsed(e) end
			if g_collection_count[e].completion == 2 then ActivateIfUsed(e) end
		end
	end
	if g_collection_count_time > 0 and g_collection_count_counts > 0 then
		g_collection_count_time = g_collection_count_time - 3.600
		Prompt(g_collection_count_counts.."/"..g_collection_count[e].objectives	.." completed " ..math.floor(g_collection_count_time/1000).. " seconds left")
	end
	if g_collection_count_time <= 0 then
		LoseGame()
	end
end
