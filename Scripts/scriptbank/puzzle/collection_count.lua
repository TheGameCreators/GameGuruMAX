-- Collection Count v11 by Necrym59 and Lee
-- DESCRIPTION: This behavior allows for a Collection Item for pickup
-- DESCRIPTION: Setup a Collection Control behavior for a collection configuration
-- DESCRIPTION: [OBJECTIVES=0(0,100)]
-- DESCRIPTION: [PICKUP_RANGE=100(0,100)]
-- DESCRIPTION: [PROMPT_DURATION=0(0,100)]
-- DESCRIPTION: [TIME_BONUS=0(0,100)] in seconds
-- DESCRIPTION: [TIME_PENALTY=0(0,100)] in seconds
-- DESCRIPTION: [HEALTH_BONUS=0(0,100)] in units
-- DESCRIPTION: [HEALTH_PENALTY=0(0,100)] in units
-- DESCRIPTION: [GLOBAL_BONUS=0(0,100)] in units
-- DESCRIPTION: [GLOBAL_PENALTY=0(0,100)] in units
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] User Global eg: "MyPoints" 
-- DESCRIPTION: <Sound0> plays when objective picked up

g_collection_count		= {}
g_collection_objectives	= 0
g_collection_time		= 0
g_collection_counted	= 0

local currentvalue		= {}

function collection_count_properties(e, objectives, pickup_range, prompt_duration, time_bonus, time_penalty, health_bonus, health_penalty, global_bonus, global_penalty, user_global_affected)
	g_collection_count[e].objectives = objectives or 0 --legacy
	g_collection_count[e].pickup_range = pickup_range or 100 --legacy
	g_collection_count[e].prompt_duration = prompt_duration or 0 --legacy
	if g_collection_count[e].objectives > 0 then g_collection_objectives = g_collection_count[e].objectives end
	if g_collection_count[e].pickup_range < 50 then g_collection_count[e].pickup_range = 50 end
	g_collection_count[e].time_bonus = time_bonus or 0
	g_collection_count[e].time_penalty = time_penalty or 0
	g_collection_count[e].health_bonus = health_bonus or 0
	g_collection_count[e].health_penalty = health_penalty or 0
	g_collection_count[e].global_bonus = global_bonus or 0
	g_collection_count[e].global_penalty = global_penalty or 0
	g_collection_count[e].user_global_affected = user_global_affected
end

function collection_count_init(e)
	g_collection_count[e] = {}
	g_collection_count[e].objectives = 0
	g_collection_count[e].pickup_range = 100
	g_collection_count[e].prompt_duration = 0
	g_collection_count[e].time_bonus = 0
	g_collection_count[e].time_penalty = 0
	g_collection_count[e].health_bonus = 0
	g_collection_count[e].health_penalty = 0
	g_collection_count[e].global_bonus = 0
	g_collection_count[e].global_penalty = 0
	g_collection_count[e].user_global_affected = ""
	currentvalue[e] = 0
	g_collection_count[e].status = "init"
end

function collection_count_main(e)
	if g_collection_count[e].status == "init" then
		g_collection_counted = 0
		g_collection_count[e].status = "endinit"
	end
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist <= g_collection_count[e].pickup_range then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if g_collection_count[e].time_bonus > 0 then g_collection_time = g_collection_time + (g_collection_count[e].time_bonus * 1000) end
		if g_collection_count[e].time_penalty > 0 then g_collection_time = g_collection_time - (g_collection_count[e].time_penalty * 1000) end
		if g_collection_count[e].health_bonus > 0 then
			SetPlayerHealth(g_PlayerHealth + g_collection_count[e].health_bonus)
			if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
			SetPlayerHealthCore(g_PlayerHealth)
		end
		if g_collection_count[e].health_penalty > 0 then
			SetPlayerHealth(g_PlayerHealth - g_collection_count[e].health_penalty)
			SetPlayerHealthCore(g_PlayerHealth)
		end
		if g_collection_count[e].user_global_affected ~= "" and g_collection_count[e].user_global_affected ~= nil then
			if g_collection_count[e].global_bonus > 0 then
				if _G["g_UserGlobal['"..g_collection_count[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..g_collection_count[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..g_collection_count[e].user_global_affected.."']"] = currentvalue[e] + g_collection_count[e].global_bonus
			end
			if g_collection_count[e].global_penalty > 0 then
				if _G["g_UserGlobal['"..g_collection_count[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..g_collection_count[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..g_collection_count[e].user_global_affected.."']"] = currentvalue[e] - g_collection_count[e].global_penalty
			end
		end
		if g_collection_counted ~= g_collection_objectives then
			g_collection_counted = g_collection_counted + 1
			if g_collection_count[e].objectives > 0 then
				if g_collection_counted == g_collection_objectives then
					JumpToLevelIfUsed(e)
				else 
					if g_collection_count[e].prompt_duration > 0 then
						PromptDuration(g_collection_counted.."/"..g_collection_objectives.." complete", g_collection_count[e].prompt_duration)
					end
				end
			end
		end
		Destroy(e)
	end
end
