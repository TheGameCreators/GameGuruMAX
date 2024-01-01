-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Alarm v4   by Necrym59
-- DESCRIPTION: When activated an alarm is sounded and alerts nearby enemies within the selected range.
-- DESCRIPTION: Attach to an object and logic link from a switch or zone to activate.
-- DESCRIPTION: [PROMPT_TEXT$="Alarm activated"]
-- DESCRIPTION: [ALARM_RESET=15(1,60)] Seconds
-- DESCRIPTION: [ALERT_RANGE=2000(1,5000)]
-- DESCRIPTION: <Sound0> - Alarm Sound

local alarm 			= {}
local prompt_text 		= {}
local alarm_reset		= {}
local alert_range		= {}

local played			= {}
local status			= {}
local reset				= {}
local state				= {}
	
function alarm_properties(e, prompt_text, alarm_reset, alert_range)
	alarm[e] = g_Entity[e]
	alarm[e].prompt_text = prompt_text
	alarm[e].alarm_reset = alarm_reset
	alarm[e].alert_range = alert_range
end
 
function alarm_init(e)
	alarm[e] = {}
	alarm[e].prompt_text = "Alarm activated"
	alarm[e].alert_range = 1000
	played[e] = 0
	state[e] = 0
	reset[e] = math.huge
	status[e] = "init"
end
 
function alarm_main(e)	
	
	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then		
		if played[e] == 0 then
			PromptDuration(alarm[e].prompt_text,3000)
			LoopSound(e,0)
			PerformLogicConnections(e)			
			reset[e] = g_Time + (alarm[e].alarm_reset*1000)
			state[e] = 1
			played[e] = 1
		end
		MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,alarm[e].alert_range,1,-1)
	end	
	if g_Time > reset[e] and state[e] == 1 then
		StopSound(e,0)
		played[e] = 0
		SetActivated(e,0)
		state[e] = 0
		status[e] = "init"
	end
end
 
function alarm_exit(e)	
end

