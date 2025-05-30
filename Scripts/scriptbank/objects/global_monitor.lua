-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Monitor v12
-- DESCRIPTION: Behavior to add or deduct a user global once or incrementally over time when activated.
-- DESCRIPTION: Attach to an object. 
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [ADJUST_AMOUNT=1] units
-- DESCRIPTION: [TIME_INTERVAL=30] seconds (if 0 will apply only once)
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [ActivateAtStart!=1] if unchecked use a switch or zone to activate

local global_monitor 		= {}
local user_global_affected	= {}
local adjust_amount			= {}
local time_interval			= {}
local effect				= {}
local ActivateAtStart		= {}

local currentvalue			= {}
local timecheck				= {}
local initialvalue			= {}
local status 				= {}
local doonce 				= {}

function global_monitor_properties(e, user_global_affected, adjust_amount, time_interval, effect, ActivateAtStart)
	global_monitor[e].user_global_affected = user_global_affected
	global_monitor[e].adjust_amount = adjust_amount
	global_monitor[e].time_interval = time_interval
	global_monitor[e].effect = effect
	global_monitor[e].ActivateAtStart = ActivateAtStart
end

function global_monitor_init(e)
	global_monitor[e] = {}
	global_monitor[e].user_global_affected = ""
	global_monitor[e].adjust_amount = 1
	global_monitor[e].time_interval = 30
	global_monitor[e].effect = 1
	global_monitor[e].ActivateAtStart = 1

	status[e] = "init"
	doonce[e] = 0
	currentvalue[e] = 0
	initialvalue[e] = 0
	timecheck[e] = 0
end

function global_monitor_main(e)

	if status[e] == "init" then
		if global_monitor[e].ActivateAtStart == 1 then SetActivated(e,1) end
		if global_monitor[e].ActivateAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		if doonce[e] == 0 then
			timecheck[e] = g_Time + (global_monitor[e].time_interval)*1000
			if _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] ~= nil then initialvalue[e] = _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] end
			doonce[e] = 1
		end
		if global_monitor[e].time_interval == 0 then
			if _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] end
			if global_monitor[e].effect == 1 then
				_G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = currentvalue[e] + global_monitor[e].adjust_amount
			end
			if global_monitor[e].effect == 2 then
				_G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = currentvalue[e] - global_monitor[e].adjust_amount
				if currentvalue[e] <= 0 then _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = 0 end
			end
			SetActivated(e,0)
		end
		if g_Time > timecheck[e] and global_monitor[e].time_interval > 0 then
			if _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] end
			if global_monitor[e].effect == 1 then
				_G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = currentvalue[e] + global_monitor[e].adjust_amount
			end
			if global_monitor[e].effect == 2 then
				_G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = currentvalue[e] - global_monitor[e].adjust_amount
				if currentvalue[e] <= 0 then _G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = 0 end
			end
			timecheck[e] = g_Time + (global_monitor[e].time_interval)*1000
		end
		--player has expired so reset the global--
		if g_PlayerHealth <= 0 then
			_G["g_UserGlobal['"..global_monitor[e].user_global_affected.."']"] = initialvalue[e]
		end
	end
end