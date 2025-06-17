-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Activator v7  by Necrym59
-- DESCRIPTION: Activator to trigger other entities or post a message if a monitored Global or User Global is of a set value. Attach to an entity.
-- DESCRIPTION: [@ACTIVATION_STYLE=1(1=Switch/Zone, 2=Ranged, 3=Always On)]
-- DESCRIPTION: [#ACTIVATION_RANGE=0(0,100)]
-- DESCRIPTION: [@@GLOBAL_TO_MONITOR$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [TRIGGER_VALUE=50(1,100)] value to triggered event
-- DESCRIPTION: [TRIGGER_DELAY=0(0,100)] in seconds to delay triggered event.
-- DESCRIPTION: [@VISIBILITY=1(1=Hide this object, 2=Show this object)]
-- DESCRIPTION: [@TRIGGER_MONITOR=1(1=Global >= Trigger Value, 2=Global <= Trigger Value)]
-- DESCRIPTION: [@ACTIVATOR_TYPE=1(1=Single Use, 2=Multi Use)]
-- DESCRIPTION: [MESSAGE_TEXT$="WARNING"] Message to send to a message global
-- DESCRIPTION: [@@MESSAGE_GLOBAL$=""(0=globallist)] eg: MyWarningMessage
-- DESCRIPTION: <Sound0> when activating

local glactivator 		= {}
local activation_style	= {}
local activation_range	= {}
local global_to_monitor	= {}
local trigger_value		= {}
local trigger_delay		= {}
local trigger_monitor	= {}
local activator_type	= {}
local message_text		= {}
local message_global	= {}

local status		= {}
local currentvalue	= {}
local played		= {}
local trigdelay		= {}

function global_activator_properties(e, activation_style, activation_range, global_to_monitor, trigger_value, trigger_delay, visibility, trigger_monitor, activator_type, message_text, message_global)
	glactivator[e].activation_style = activation_style
	glactivator[e].activation_range = activation_range
	glactivator[e].global_to_monitor = global_to_monitor
	glactivator[e].trigger_value = trigger_value
	glactivator[e].trigger_delay = trigger_delay
	glactivator[e].visibility = visibility
	glactivator[e].trigger_monitor = trigger_monitor
	glactivator[e].activator_type = activator_type
	glactivator[e].message_text	= message_text
	glactivator[e].message_global = message_global
end

function global_activator_init(e)
	glactivator[e] = {}
	glactivator[e].activation_style = 1
	glactivator[e].activation_range = 0
	glactivator[e].global_to_monitor = ""
	glactivator[e].trigger_value = 50
	glactivator[e].trigger_delay = 0
	glactivator[e].visibility = 1
	glactivator[e].trigger_monitor = 1
	glactivator[e].activator_type = 1
	glactivator[e].message_text	= "WARNING"
	glactivator[e].message_global = ""

	status[e] = "init"
	currentvalue[e] = 0
	played[e] = 0
	trigdelay[e] = math.huge
end

function global_activator_main(e)
	if status[e] == "init" then
		if glactivator[e].trigger_delay ~= nil then trigdelay[e] = glactivator[e].trigger_delay * 1000 end
		if glactivator[e].visibility == 1 then
			CollisionOff(e)
			Hide(e)
		end
		status[e] = "endinit"
	end
	if glactivator[e].activation_style == 2 then
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < glactivator[e].activation_range then
			SetActivated(e,1)
		else
			SetActivated(e,0)
		end
	end
	if glactivator[e].activation_style == 3 then
		SetActivated(e,1)
	end

	if g_Entity[e]['activated'] == 1 then
		if glactivator[e].global_to_monitor > "" then
			if _G["g_UserGlobal['"..glactivator[e].global_to_monitor.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..glactivator[e].global_to_monitor.."']"] end
			if glactivator[e].trigger_monitor == 1 then
				if currentvalue[e] >= glactivator[e].trigger_value then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = glactivator[e].message_text
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end
					if glactivator[e].activator_type == 1 then
						if glactivator[e].visibility == 1 then Destroy(e) end
						if glactivator[e].visibility == 2 then SwitchScript(e,"no_behavior_selected.lua") end
					end
					if glactivator[e].activator_type == 2 then
						currentvalue[e] = 0
						_G["g_UserGlobal['"..glactivator[e].global_to_monitor.."']"] = currentvalue[e]
						SetActivated(e,0)
					end
				end
			end
			if glactivator[e].trigger_monitor == 2 then
				if currentvalue[e] <= glactivator[e].trigger_value then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
					_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = glactivator[e].message_text
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end
					if glactivator[e].activator_type == 1 then
						if glactivator[e].visibility == 1 then Destroy(e) end
						if glactivator[e].visibility == 2 then SwitchScript(e,"no_behavior_selected.lua") end
					end
					if glactivator[e].activator_type == 2 then
						currentvalue[e] = 0
						_G["g_UserGlobal['"..glactivator[e].global_to_monitor.."']"] = currentvalue[e]
						SetActivated(e,0)
					end
				end
			end
		end
	end
end