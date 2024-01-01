-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Monitor v6 by Necrym59
-- DESCRIPTION: A global behavior that allows a named npc to be
-- DESCRIPTION: health monitored and trigger event(s) or Lose Game or Win game upon death.
-- DESCRIPTION: Attach to an object set AlwaysActive=ON
-- DESCRIPTION: and attach any logic links to this object
-- DESCRIPTION: and/or use ActivateIfUsed field.
-- DESCRIPTION: [NPC_NAME$=""] to monitor.
-- DESCRIPTION: [@DEATH_ACTION=1(1=Event Triggers, 2=Lose Game, 3=Win Game)]
-- DESCRIPTION: [@DISPLAY_HEALTH=2(1=Yes, 2=No)]
-- DESCRIPTION: [MONITOR_ACTIVE!=1] if unchecked use a switch or other trigger to activate/decativate this object

local lower = string.lower

local npc_monitor 		= {}
local npc_name			= {}
local death_action		= {}
local display_health	= {}
local monitor_active	= {}

local pEntn				= {}
local status			= {}
local wait				= {}

function npc_monitor_properties(e, npc_name, death_action, display_health, monitor_active)
	npc_monitor[e] = g_Entity[e]
	npc_monitor[e].npc_name = lower(npc_name) or ""
	npc_monitor[e].death_action = death_action
	npc_monitor[e].display_health = display_health
	npc_monitor[e].monitor_active = monitor_active
end

function npc_monitor_init(e)
	npc_monitor[e] = {}
	npc_monitor[e].npc_name = ""
	npc_monitor[e].death_action = 1	
	npc_monitor[e].display_health = 2
	npc_monitor[e].monitor_active = 1
	status[e] = "init"
	wait[e] = math.huge
	pEntn[e] = 0
end

function npc_monitor_main(e)
	npc_monitor[e] = g_Entity[e]

	if status[e] == "init" then
		if npc_monitor[e].monitor_active == 1 then SetEntityActivated(e,1) end
		if npc_monitor[e].monitor_active == 0 then SetEntityActivated(e,0) end	
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
				wait[e] = g_Time + 5000
				ActivateIfUsed(e)
				PerformLogicConnections(e)
				status[e] = "alarm"
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_monitor[e].death_action == 2 then
				LoseGame()
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_monitor[e].death_action == 3 then
				WinGame()
			end
			if npc_monitor[e].display_health == 1 then
				TextCenterOnX(50,5,1,"Monitored NPC Health: " ..g_Entity[pEntn[e]].health)
			end
		end

		if status[e] == "alarm" then
			if g_Time < wait[e] then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,3000,1,-1) end
			if g_Time > wait[e] then
				status[e] = "end"
				g_Entity[e]['activated'] = 0
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end
	end
end