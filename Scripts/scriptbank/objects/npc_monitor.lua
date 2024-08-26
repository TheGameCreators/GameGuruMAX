-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Monitor v10 by Necrym59
-- DESCRIPTION: A global behavior that allows a named npc to be health monitored and trigger event(s) or Lose/Win game or go to a specified level upon its death.
-- DESCRIPTION: Attach to an object set AlwaysActive=ON, and attach any logic links to this object and/or use ActivateIfUsed field.
-- DESCRIPTION: [NPC_NAME$=""] to monitor.
-- DESCRIPTION: [@DEATH_ACTION=1(1=Event Triggers, 2=Lose Game, 3=Win Game, 4=Go To Level)]
-- DESCRIPTION: [@DISPLAY_HEALTH=2(1=Yes, 2=No)] to display health on npc
-- DESCRIPTION: [@MONITOR_ACTIVE=1(1=Yes, 2=No)] if No then use a zone or switch to activate this monitor.
-- DESCRIPTION: [ACTION_DELAY=2(0,100)] seconds delay before activating death action.
-- DESCRIPTION: [USER_GLOBAL$=""] user global to apply value (eg: MyGlobal).
-- DESCRIPTION: [USER_GLOBAL_VALUE=100(1,100)] value to apply.
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded.
-- DESCRIPTION: [ResetStates!=0] when entering the new level

local lower = string.lower

local npc_monitor 		= {}
local npc_name			= {}
local death_action		= {}
local display_health	= {}
local monitor_active	= {}
local action_delay		= {}
local user_global 		= {}
local user_global_value	= {}
local resetstates		= {}

local pEntn				= {}
local status			= {}
local wait				= {}

function npc_monitor_properties(e, npc_name, death_action, display_health, monitor_active, action_delay, user_global, user_global_value, resetstates)
	npc_monitor[e] = g_Entity[e]
	npc_monitor[e].npc_name = lower(npc_name) or ""
	npc_monitor[e].death_action = death_action
	npc_monitor[e].display_health = display_health
	npc_monitor[e].monitor_active = monitor_active or 1
	npc_monitor[e].action_delay = action_delay or 0	
	npc_monitor[e].user_global = user_global
	npc_monitor[e].user_global_value = user_global_value
	npc_monitor[e].resetstates = resetstates
end

function npc_monitor_init(e)
	npc_monitor[e] = {}
	npc_monitor[e].npc_name = ""
	npc_monitor[e].death_action = 1	
	npc_monitor[e].display_health = 2
	npc_monitor[e].monitor_active = 1
	npc_monitor[e].action_delay = 3		
	npc_monitor[e].user_global = ""
	npc_monitor[e].user_global_value = 100
	npc_monitor[e].resetstates = 0
	
	status[e] = "init"
	entheight[e] = 0
	wait[e] = math.huge
	actiondelay[e] = math.huge
	pEntn[e] = 0
end

function npc_monitor_main(e)
	npc_monitor[e] = g_Entity[e]

	if status[e] == "init" then
		if npc_monitor[e].monitor_active == 1 then SetEntityActivated(e,1) end
		if npc_monitor[e].monitor_active == 2 then SetEntityActivated(e,0) end	
		pEntn[e] = 0
		if pEntn[e] == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == npc_monitor[e].npc_name then 
						pEntn[e] = n
						status[e] = "monitor"
						break
					end
				end
			end
		end
	end	

	if g_Entity[e]['activated'] == 1 then
	
		if status[e] == "monitor" then
			if g_Entity[pEntn[e]].health <= 0 and npc_monitor[e].death_action == 1 then
				wait[e] = g_Time + (npc_monitor[e].action_delay*1000)				
				if npc_monitor[e].user_global > "" then
					_G["g_UserGlobal['"..npc_monitor[e].user_global.."']"] = npc_monitor[e].user_global_value
				end
				status[e] = "alarm"
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_monitor[e].death_action == 2 then
				wait[e] = g_Time + (npc_monitor[e].action_delay*1000)
				status[e] = "winorlose"
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_monitor[e].death_action == 3 then			
				wait[e] = g_Time + (npc_monitor[e].action_delay*1000)
				if npc_monitor[e].user_global > "" then
					_G["g_UserGlobal['"..npc_monitor[e].user_global.."']"] = npc_monitor[e].user_global_value
				end
				status[e] = "winorlose"
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_monitor[e].death_action == 4 then			
				wait[e] = g_Time + (npc_monitor[e].action_delay*1000)
				if npc_monitor[e].user_global > "" then
					_G["g_UserGlobal['"..npc_monitor[e].user_global.."']"] = npc_monitor[e].user_global_value
				end
				status[e] = "winorlose"
			end
			if npc_monitor[e].display_health == 1 then
				PromptLocal(pEntn[e],"Health: " ..g_Entity[pEntn[e]].health)
			end
		end

		if status[e] == "alarm" then			
			if g_Time < wait[e] then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,3000,1,-1) end
			if g_Time > wait[e] then
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				status[e] = "end"
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end
		
		if status[e] == "winorlose" then
			if g_Time > wait[e] then
				if npc_monitor[e].death_action == 2 then LoseGame() end
				if npc_monitor[e].death_action == 3 then WinGame() end
				if npc_monitor[e].death_action == 4 then
					JumpToLevelIfUsedEx(e,npc_monitor[e].resetstates)
				end
				status[e] = "end"
			end
		end
	end
end