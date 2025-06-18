-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Activator v7  by Necrym59
-- DESCRIPTION: Activator to trigger other entities or post a message if a monitored Global or User Global is of a set value. Attach to an entity.
-- DESCRIPTION: [@ACTIVATION_STYLE=1(1=Switch/Zone,2=Is-Active)]
-- DESCRIPTION: [@ACTIVATION_MODE=1(1=Single Use, 2=Multi Use)]
-- DESCRIPTION: [@@GLOBAL_TO_MONITOR$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [TRIGGER_VALUE=20(1,100)] value to trigger event
-- DESCRIPTION: [@TRIGGER_MONITOR=1(1=Global >= Trigger Value, 2=Global <= Trigger Value)]
-- DESCRIPTION: [MESSAGE_TEXT$="WARNING"] Message to send to a message global
-- DESCRIPTION: [@@MESSAGE_GLOBAL$=""(0=globallist)] eg: MyWarningMessage
-- DESCRIPTION: [@VISIBILITY=1(1=Hide this entity, 2=Show this entity)]
-- DESCRIPTION: <Sound0> when activating

local glactivator 		= {}
local activation_style	= {}
local activation_mode	= {}
local global_to_monitor	= {}
local trigger_value		= {}
local trigger_monitor	= {}
local message_text		= {}
local message_global	= {}
local visibility		= {}

local status		= {}
local currentvalue	= {}
local played		= {}
local triged		= {}
local multiswitch	= {}

function global_activator_properties(e, activation_style, activation_mode, global_to_monitor, trigger_value, trigger_monitor, message_text, message_global, visibility)
	glactivator[e].activation_style = activation_style
	glactivator[e].activation_mode = activation_mode	
	glactivator[e].global_to_monitor = global_to_monitor
	glactivator[e].trigger_value = trigger_value
	glactivator[e].trigger_monitor = trigger_monitor
	glactivator[e].message_text	= message_text
	glactivator[e].message_global = message_global
	glactivator[e].visibility = visibility	
end

function global_activator_init(e)
	glactivator[e] = {}
	glactivator[e].activation_style = 1
	glactivator[e].activation_mode = 1	
	glactivator[e].global_to_monitor = ""
	glactivator[e].trigger_value = 50
	glactivator[e].trigger_monitor = 1
	glactivator[e].message_text	= "WARNING"
	glactivator[e].message_global = ""
	glactivator[e].visibility = 1	

	status[e] = "init"
	currentvalue[e] = 0
	played[e] = 0
	triged[e] = 0
	multiswitch[e] = 0
end

function global_activator_main(e)

	if status[e] == "init" then
		if glactivator[e].visibility == 1 then
			CollisionOff(e)
			Hide(e)
		end
		if glactivator[e].activation_style == 1 then SetActivated(e,0) end		
		if glactivator[e].activation_style == 2 then SetActivated(e,1) end
		SetEntityAlwaysActive(e,1)
		_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = ""
		triged[e] = 0
		played[e] = 0		
		status[e] = "endinit"
	end	
	
	if g_Entity[e]['activated'] == 1 then
		if glactivator[e].global_to_monitor == "" then Text(50,50,3,"No Global to Monitor") end
		if glactivator[e].global_to_monitor ~= "" then
			--if _G["g_UserGlobal['"..glactivator[e].global_to_monitor.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..glactivator[e].global_to_monitor.."']"] end			
			currentvalue[e] = g_PlayerGunAmmoCount -- TEMP ONLY
			
			--Single Use --
			if glactivator[e].activation_mode == 1 then
				if glactivator[e].trigger_monitor == 1 then -- Greater Than --
					if currentvalue[e] >= glactivator[e].trigger_value then
						if triged[e] == 0 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)
							triged[e] = 1
						end	
						_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = glactivator[e].message_text	
						PlaySound(e,0)
						if glactivator[e].visibility == 1 then Destroy(e) end
						if glactivator[e].visibility == 2 then SwitchScript(e,"no_behavior_selected.lua") end					
					end
				end
				if glactivator[e].trigger_monitor == 2 then -- Smaller Than --
					if currentvalue[e] <= glactivator[e].trigger_value then
						if triged[e] == 0 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)							
							triged[e] = 1
						end	
						_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = glactivator[e].message_text	
						PlaySound(e,0)
						if glactivator[e].visibility == 1 then Destroy(e) end
						if glactivator[e].visibility == 2 then SwitchScript(e,"no_behavior_selected.lua") end
					end	
				end
			end
			
			--Multi Use --			
			if glactivator[e].activation_mode == 2 and multiswitch[e] == 0 then				
				if glactivator[e].trigger_monitor == 1 then -- Greater Than --
					if currentvalue[e] >= glactivator[e].trigger_value then
						if triged[e] == 0 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)							
							PlaySound(e,0)
							triged[e] = 1	
						end
						_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = glactivator[e].message_text	
						multiswitch[e] = 1
					end
				end	
				if glactivator[e].trigger_monitor == 2 then -- Smaller Than --
					if currentvalue[e] <= glactivator[e].trigger_value then
						if triged[e] == 0 then
							PerformLogicConnections(e)
							ActivateIfUsed(e)
							PlaySound(e,0)
							triged[e] = 1
						end
						_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = glactivator[e].message_text	
						multiswitch[e] = 1
					end				
				end	
			end
			
			--Multi Use Reset --
			if glactivator[e].activation_mode == 2 and multiswitch[e] == 1 then
				if glactivator[e].trigger_monitor == 1 and currentvalue[e] <= glactivator[e].trigger_value then
					_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = ""
					multiswitch[e] = 0
				end
				if glactivator[e].trigger_monitor == 2 and currentvalue[e] >= glactivator[e].trigger_value then	
					_G["g_UserGlobal['"..glactivator[e].message_global.."']"] = ""
					multiswitch[e] = 0
				end
			end
		end		
	end
	Text(50,80,3,multiswitch[e])
end  