-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Activator v4  by Necrym59
-- DESCRIPTION: An activator to trigger other logic linked or IfUsed entities if the User Global is of a set value.
-- DESCRIPTION: Attach to an entity. Trigger by switch, zone or range.
-- DESCRIPTION: [@ACTIVATION_STYLE=1(1=Switch/Zone, 2=Range)]
-- DESCRIPTION: [#ACTIVATION_RANGE=0(0,100)]
-- DESCRIPTION: [USER_GLOBAL$=""] eg: MyGlobal
-- DESCRIPTION: [TRIGGER_VALUE=50(1,100)] value to triggered event
-- DESCRIPTION: [TRIGGER_DELAY=0(0,100)] in seconds to delay triggered event.
-- DESCRIPTION: [@VISIBILITY=1(1=Hide, 2=Show)]
-- DESCRIPTION: [@TRIGGER_MONITOR=1(1=>= trigger value, 2=<= trigger value)]
-- DESCRIPTION: <Sound0> when activating

local glactivator 		= {}
local activation_style	= {}
local activation_range	= {}
local user_global 		= {}
local trigger_value		= {}
local trigger_delay		= {}
local trigger_monitor	= {}

local status		= {}
local currentvalue	= {}
local played		= {}
local trigdelay		= {}

function global_activator_properties(e, activation_style, activation_range, user_global, trigger_value, trigger_delay, visibility, trigger_monitor)
	glactivator[e] = g_Entity[e]
	glactivator[e].activation_style = activation_style
	glactivator[e].activation_range = activation_range	
	glactivator[e].user_global = user_global
	glactivator[e].trigger_value = trigger_value
	glactivator[e].trigger_delay = trigger_delay
	glactivator[e].visibility = visibility
	glactivator[e].trigger_monitor = trigger_monitor
end

function global_activator_init(e)
	glactivator[e] = {}
	glactivator[e].activation_style = 1
	glactivator[e].activation_range = 0
	glactivator[e].user_global = ""
	glactivator[e].trigger_value = 50
	glactivator[e].trigger_delay = 0
	glactivator[e].visibility = 1
	glactivator[e].trigger_monitor = 1	
	
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
	local PlayerDist = GetPlayerDistance(e)
	if glactivator[e].activation_style == 2 then
		if PlayerDist < glactivator[e].activation_range then
			SetActivated(e,1)
		else
			SetActivated(e,0)
		end
	end
	if g_Entity[e]['activated'] == 1 then
		if glactivator[e].user_global > "" then
			if _G["g_UserGlobal['"..glactivator[e].user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..glactivator[e].user_global.."']"] end
			if glactivator[e].trigger_monitor == 1 then 
				if currentvalue[e] >= glactivator[e].trigger_value then			
					PerformLogicConnections(e)
					ActivateIfUsed(e)			
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end	
					if glactivator[e].visibility == 1 then Destroy(e) end
					if glactivator[e].visibility == 2 then SwitchScript(e,"no_behavior_selected.lua") end
				end
			end
			if glactivator[e].trigger_monitor == 2 then 
				if currentvalue[e] <= glactivator[e].trigger_value then			
					PerformLogicConnections(e)
					ActivateIfUsed(e)			
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end	
					if glactivator[e].visibility == 1 then Destroy(e) end
					if glactivator[e].visibility == 2 then SwitchScript(e,"no_behavior_selected.lua") end
				end
			end				
		end
	end	
end