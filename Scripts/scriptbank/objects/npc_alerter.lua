-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- NPC Alerter v3 by Necrym59
-- DESCRIPTION: A global behavior that allows a named npc to be an alerter of other npc's and can trigger event(s) or Lose Game or Win game upon death.
-- DESCRIPTION: Attach to an object set AlwaysActive=ON. Attach any logic links to this object and/or use ActivateIfUsed field.
-- DESCRIPTION: [NPC_NAME$=""] to be the Alerter.
-- DESCRIPTION: [@ALERT_ACTION=1(1=Call Alert, 2=Event Trigger, 3=Call Alert+EventTrigger, 4=Lose Game, 5=Win Game, 6=None)]
-- DESCRIPTION: [ALERT_RANGE=3000(1,5000)]
-- DESCRIPTION: [@DEATH_ACTION=4(1=Event Trigger, 2=Lose Game, 3=Win Game, 4=None)]
-- DESCRIPTION: [ALERTER_ACTIVE!=1] if unchecked use a switch or other trigger to activate/decativate this object
-- DESCRIPTION: <Sound0> for alert sound

local lower = string.lower

local npc_alerter 		= {}
local npc_name			= {}
local alert_action		= {}
local death_action		= {}
local alerter_active	= {}

local pEntn				= {}
local status			= {}
local doonce			= {}
local doonce2			= {}
local played			= {}
local wait				= {}
local wait2				= {}

function npc_alerter_properties(e, npc_name, alert_action, alert_range, death_action, alerter_active)
	npc_alerter[e] = g_Entity[e]
	npc_alerter[e].npc_name = lower(npc_name)
	npc_alerter[e].alert_action = alert_action
	npc_alerter[e].alert_range = alert_range
	npc_alerter[e].death_action = death_action
	npc_alerter[e].alerter_active = alerter_active
end

function npc_alerter_init(e)
	npc_alerter[e] = {}
	npc_alerter[e].npc_name = ""
	npc_alerter[e].alert_action = 2	
	npc_alerter[e].alert_range = 3000	
	npc_alerter[e].death_action = 1	
	npc_alerter[e].alerter_active = 1
	status[e] = "init"
	wait[e] = math.huge
	wait2[e] = math.huge
	doonce[e] = 0
	doonce2[e] = 0	
	played[e] = 0
	pEntn[e] = 0
end

function npc_alerter_main(e)
	npc_alerter[e] = g_Entity[e]

	if status[e] == "init" then
		if npc_alerter[e].alerter_active == 1 then SetActivated(e,1) end
		if npc_alerter[e].alerter_active == 0 then SetActivated(e,0) end	
		pEntn[e] = 0
		if pEntn[e] == 0 then
			for n = 1, g_EntityElementMax do
				if n ~= nil and g_Entity[n] ~= nil then
					if lower(GetEntityName(n)) == npc_alerter[e].npc_name then 
						pEntn[e] = n
						status[e] = "observe-alert"
						break
					end
				end
			end
		end
	end	

	if g_Entity[e]['activated'] == 1 then
	
		if status[e] == "observe-alert" then
			if npc_alerter[e].alert_action == 1 then
				GetEntityPlayerVisibility(e)
				if g_Entity[pEntn[e]]['plrvisible'] == 1 then
					MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,npc_alerter[e].alert_range,1,-1)
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end	
				end			
			end	
			if npc_alerter[e].alert_action == 2 then
				GetEntityPlayerVisibility(e)
				if g_Entity[pEntn[e]]['plrvisible'] == 1 then
					if doonce[e] == 0 then
						ActivateIfUsed(e)
						PerformLogicConnections(e)						
						doonce[e] = 1
					end		
				end
			end
			if npc_alerter[e].alert_action == 3 then
				GetEntityPlayerVisibility(e)
				if g_Entity[pEntn[e]]['plrvisible'] == 1 then
					MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,npc_alerter[e].alert_range,1,-1)
					if doonce[e] == 0 then
						ActivateIfUsed(e)
						PerformLogicConnections(e)						
						doonce[e] = 1
					end
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end						
				end
			end
			if npc_alerter[e].alert_action == 4 then
				if g_Entity[pEntn[e]]['plrvisible'] == 1 then
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end	
					if doonce2[e] == 0 then
						wait2[e] = g_Time + 2000
						doonce2[e] = 1
					end	
				end	
			end
			if npc_alerter[e].alert_action == 5 then
				if g_Entity[pEntn[e]]['plrvisible'] == 1 then
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end					
					if doonce2[e] == 0 then
						wait2[e] = g_Time + 2000
						doonce2[e] = 1
					end							
				end
			end			
			if g_Entity[pEntn[e]]['plrvisible'] == 0 then played[e] = 0	end	
			
			if g_Entity[pEntn[e]].health <= 0 and npc_alerter[e].death_action == 1 then
				wait[e] = g_Time + 5000
				if npc_alerter[e].alert_action ~= 2 or npc_alerter[e].alert_action ~= 3 then
					ActivateIfUsed(e)
					PerformLogicConnections(e)
				end	
				status[e] = "dying"
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_alerter[e].death_action == 2 then
				LoseGame()
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_alerter[e].death_action == 3 then
				WinGame()
			end
			if g_Entity[pEntn[e]].health <= 0 and npc_alerter[e].death_action == 4 then
				wait[e] = g_Time + 5000
				status[e] = "dying"
			end
		end

		if status[e] == "dying" then
			if g_Time < wait[e] then MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,npc_alerter[e].alert_range,1,-1) end
			if g_Time > wait[e] then
				status[e] = "end"
				g_Entity[e]['activated'] = 0
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end
		
		if g_Time > wait2[e] then
			if npc_alerter[e].alert_action == 4 then LoseGame() end
			if npc_alerter[e].alert_action == 5 then WinGame() end
			g_Entity[e]['activated'] = 0
			SwitchScript(e,"no_behavior_selected.lua")
		end
	end
end