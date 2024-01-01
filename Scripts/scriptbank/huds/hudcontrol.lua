-- Hud Control v2   by Necrym59
-- DESCRIPTION: When player enters this zone will Hide or Show in-game HUD then destroy the zone.
-- DESCRIPTION: [@MODE=1(1=Show, 2=Hide)]
-- DESCRIPTION: [MODE_TEXT$="Hud disabled/enabled"]
-- DESCRIPTION: [ZONEHEIGHT=100(0,1000)]
-- DESCRIPTION: [SpawnAtStart!=1] if unchecked use a switch or other trigger to spawn this zone

	local hudcontrol	= {}
	local hud_mode		= {}
	local mode_text 	= {}
	local zoneheight	= {}
	local SpawnAtStart	= {}
	local status		= {}
	
function hudcontrol_properties(e, hud_mode, mode_text, zoneheight, SpawnAtStart)
	hudcontrol[e] = g_Entity[e]
	hudcontrol[e].hud_mode = hud_mode
	hudcontrol[e].mode_text = mode_text
	hudcontrol[e].zoneheight = zoneheight or 100
	hudcontrol[e].SpawnAtStart = SpawnAtStart
end 

function hudcontrol_init(e)
	hudcontrol[e] = g_Entity[e]
	hudcontrol[e].hud_mode = 1
	hudcontrol[e].mode_text = "Hud disabled/enabled"
	hudcontrol[e].zoneheight = 100
	hudcontrol[e].SpawnAtStart = 1
	status[e] = "init"
end

function hudcontrol_main(e)
	hudcontrol[e] = g_Entity[e]
	if status[e] == "init" then
		if hudcontrol[e].SpawnAtStart == 1 then SetActivated(e,1) end
		if hudcontrol[e].SpawnAtStart == 0 then SetActivated(e,0) end
		status[e] = "endinit"
	end
	
	if g_Entity[e]['activated'] == 1 then 	
		if g_Entity[e]['plrinzone']== 1 and g_PlayerHealth > 0 and g_PlayerPosY < g_Entity[e]['y']+hudcontrol[e].zoneheight then
			if hudcontrol[e].hud_mode == 1 then
				PromptDuration(hudcontrol[e].mode_text,1000)
				ShowHuds()
				Destroy(e)
			end
			if hudcontrol[e].hud_mode == 2 then
				PromptDuration(hudcontrol[e].mode_text,1000)
				HideHuds()
				Destroy(e)
			end
		end
	end
end
