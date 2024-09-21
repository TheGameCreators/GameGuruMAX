-- Collection Count v9 by Necrym59 and Lee
-- DESCRIPTION: This behavior allows for a Collection Item for pickup
-- DESCRIPTION: Set a Collection Control behavior for a collection configuration
-- DESCRIPTION: [PICKUP_RANGE=100(0,100)]
-- DESCRIPTION: [TIME_BONUS=0(0,100)] in seconds
-- DESCRIPTION: [TIME_PENALTY=0(0,100)] in seconds
-- DESCRIPTION: [HEALTH_BONUS=0(0,100)] in units
-- DESCRIPTION: [HEALTH_PENALTY=0(0,100)] in units
-- DESCRIPTION: [GLOBAL_BONUS=0(0,100)] in units
-- DESCRIPTION: [GLOBAL_PENALTY=0(0,100)] in units
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] User Global eg: "MyPoints" 
-- DESCRIPTION: <Sound0> plays when objective picked up

g_collection_objectives	= 0
g_collection_time		= 0
g_collection_counted	= 0

local cc_count			= {}
local pickup_range		= {}
local time_bonus		= {}
local time_penalty		= {}
local health_bonus		= {}
local health_penalty	= {}
local global_bonus		= {}
local global_penalty	= {}
local user_global_affected = {}

local currentvalue		= {}
local status			= {}

function collection_count_properties(e, pickup_range, time_bonus, time_penalty, health_bonus, health_penalty, global_bonus, global_penalty, user_global_affected)
	cc_count[e].pickup_range = pickup_range
	if cc_count[e].pickup_range < 30 then cc_count[e].pickup_range = 30 end
	cc_count[e].time_bonus = time_bonus or 0
	cc_count[e].time_penalty = time_penalty or 0
	cc_count[e].health_bonus = health_bonus or 0
	cc_count[e].health_penalty = health_penalty or 0
	cc_count[e].global_bonus = global_bonus or 0
	cc_count[e].global_penalty = global_penalty or 0
	cc_count[e].user_global_affected = user_global_affected
end

function collection_count_init(e)
	cc_count[e] = {}
	cc_count[e].pickup_range = 100
	cc_count[e].time_bonus = 0
	cc_count[e].time_penalty = 0
	cc_count[e].health_bonus = 0
	cc_count[e].health_penalty = 0
	cc_count[e].global_bonus = 0
	cc_count[e].global_penalty = 0
	cc_count[e].user_global_affected = ""

	currentvalue[e] = 0
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
		if cc_count[e].health_bonus > 0 then
			SetPlayerHealth(g_PlayerHealth + cc_count[e].health_bonus)
			if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
			SetPlayerHealthCore(g_PlayerHealth)
		end
		if cc_count[e].health_penalty > 0 then
			SetPlayerHealth(g_PlayerHealth - cc_count[e].health_penalty)
			SetPlayerHealthCore(g_PlayerHealth)
		end
		if cc_count[e].user_global_affected ~= "" and cc_count[e].user_global_affected ~= nil then
			if cc_count[e].global_bonus > 0 then
				if _G["g_UserGlobal['"..cc_count[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..cc_count[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..cc_count[e].user_global_affected.."']"] = currentvalue[e] + cc_count[e].global_bonus
			end
			if cc_count[e].global_penalty > 0 then
				if _G["g_UserGlobal['"..cc_count[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..cc_count[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..cc_count[e].user_global_affected.."']"] = currentvalue[e] - cc_count[e].global_penalty
			end
		end
		if g_collection_counted ~= g_collection_objectives then
			g_collection_counted = g_collection_counted + 1
		end
		Destroy(e)
	end
end
