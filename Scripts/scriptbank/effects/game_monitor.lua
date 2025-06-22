-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Game Monitor v5 by Necrym59
-- DESCRIPTION: Will monitor a User Global Value and launch the designated game action.
-- DESCRIPTION: Attach to an object set AlwaysActive=On.
-- DESCRIPTION: [@@GLOBAL_TO_MONITOR$=""(0=globallist)] User Global used to monitor (eg; MyPoints)
-- DESCRIPTION: [TRIGGER_VALUE=0] Trigger Value
-- DESCRIPTION: [@TRIGGER_MONITOR=1(1=Global >= Trigger Value, 2=Global <= Trigger Value)]
-- DESCRIPTION: [@TRIGGER_ACTION=1(1=Win Game, 2=Lose Game, 3=Go To Level)]
-- DESCRIPTION: [TRIGGER_TEXT$="Press E to Continue"] For notification of trigger action.
-- DESCRIPTION: [@TRIGGER_TEXT_DISPLAY=1(1=On, 2=Off)] For pause and display of notification text.
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic, 2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded.

local gamemonitor			= {}
local global_to_monitor		= {}
local trigger_value			= {}
local trigger_monitor		= {}
local trigger_action		= {}
local trigger_text			= {}
local trigger_text_display	= {}

local currentvalue		= {}
local status			= {}
local checktimer		= {}

function game_monitor_properties(e, global_to_monitor, trigger_value, trigger_monitor, trigger_action, trigger_text, trigger_text_display)
	gamemonitor[e].global_to_monitor = global_to_monitor
	gamemonitor[e].trigger_value = trigger_value
	gamemonitor[e].trigger_monitor = trigger_monitor
	gamemonitor[e].trigger_action = trigger_action
	gamemonitor[e].trigger_text = trigger_text
	gamemonitor[e].trigger_text_display = trigger_text_display
end

function game_monitor_init(e)
	gamemonitor[e] = {}
	gamemonitor[e].global_to_monitor = ""
	gamemonitor[e].trigger_value = 0
	gamemonitor[e].trigger_monitor = 1
	gamemonitor[e].trigger_action = 1
	gamemonitor[e].trigger_text = ""
	gamemonitor[e].trigger_text_display = 1

	status[e] = "init"
	currentvalue[e] = 0
	checktimer[e] = math.huge
end

function game_monitor_main(e)
	if status[e] == "init" then
		checktimer[e] = g_Time + 250
		status[e] = "monitor"
	end

	if status[e] == "monitor" then

		if gamemonitor[e].global_to_monitor ~= "" then
			if _G["g_UserGlobal['"..gamemonitor[e].global_to_monitor.."']"] ~= "" then currentvalue[e] = _G["g_UserGlobal['"..gamemonitor[e].global_to_monitor.."']"] end
			if g_Time > checktimer[e] then
				if gamemonitor[e].trigger_monitor == 1 then
					if currentvalue[e] >= gamemonitor[e].trigger_value then	status[e] = "ending" end
				end
				if gamemonitor[e].trigger_monitor == 2 then
					if currentvalue[e] <= gamemonitor[e].trigger_value then	status[e] = "ending" end
				end
				checktimer[e] = g_Time + 250
			end
		end
	end

	if status[e] == "ending" then
		if gamemonitor[e].trigger_text_display == 1 then  -- Text Pause
			SetCameraOverride(3)
			PromptDuration(gamemonitor[e].trigger_text,2000)
			if g_KeyPressE == 1 then
				SetCameraOverride(0)
				if gamemonitor[e].trigger_action == 1 then WinGame() end
				if gamemonitor[e].trigger_action == 2 then LoseGame() end
				if gamemonitor[e].trigger_action == 3 then JumpToLevelIfUsedEx(e,0) end
			end
		end
		if gamemonitor[e].trigger_text_display == 2 then -- No Text Direct action
			if gamemonitor[e].trigger_action == 1 then WinGame() end
			if gamemonitor[e].trigger_action == 2 then LoseGame() end
			if gamemonitor[e].trigger_action == 3 then JumpToLevelIfUsedEx(e,0) end
		end
	end
end