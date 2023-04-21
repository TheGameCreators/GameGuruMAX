-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Alertzone v4   by Necrym59
-- DESCRIPTION: The player may alert nearby enemies within the selected range while in this Zone
-- DESCRIPTION: Link to a trigger Zone.
-- DESCRIPTION: [PROMPT_TEXT$="In Alert Zone"]
-- DESCRIPTION: [ALERT_RANGE=1000(1,5000)]
-- DESCRIPTION: [@ALERT_MODE=1(1=Multiple Use, 2=Single Use)]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone
-- DESCRIPTION: <Sound0> -In zone Effect Sound

	local alertzone 		= {}
	local prompt_text 		= {}
	local alert_range		= {}
	local alert_mode     	= {}
	local zoneheight		= {}
	local SpawnAtStart		= {}

	local played			= {}
	local alerted			= {}
	local status			= {}
	
function alertzone_properties(e, prompt_text, alert_range, alert_mode, zoneheight, SpawnAtStart)
	alertzone[e] = g_Entity[e]
	alertzone[e].prompt_text = prompt_text
	alertzone[e].alert_range = alert_range
	alertzone[e].alert_mode = alert_mode
	alertzone[e].zoneheight = zoneheight or 100
	alertzone[e].SpawnAtStart = SpawnAtStart
end
 
function alertzone_init(e)
	alertzone[e] = g_Entity[e]
	alertzone[e].prompt_text = "In Alert Zone"
	alertzone[e].alert_range = 1000
	alertzone[e].alert_mode = 1
	alertzone[e].zoneheight = 100
	alertzone[e].SpawnAtStart = 1
	played[e] = 0
	alerted[e] = 0
	status[e] = "init"
end
 
function alertzone_main(e)	
	alertzone[e] = g_Entity[e]	
	
	if status[e] == "init" then
		if alertzone[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if alertzone[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then	
		if g_Entity[e]['plrinzone'] == 1 and alerted[e] == 0 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+alertzone[e].zoneheight then				
			Prompt(alertzone[e].prompt_text)
			if played[e] == 0 then				
				PlaySound(e,0)
				MakeAISound(g_PlayerPosX,g_PlayerPosY,g_PlayerPosZ,alertzone[e].alert_range,1,e) -- (0-neutral, 1-enemy, 2-player)
				played[e] = 1
				alerted[e] = 1
				if alertzone[e].alert_mode == 2 then Destroy(e) end
			end			
		end	
		if g_Entity[e]['plrinzone'] == 0 and alerted[e] == 1 then
			StopSound(e,0)
			played[e] = 0
			alerted[e] = 0
		end
	end
end
 
function alertzone_exit(e)	
end

