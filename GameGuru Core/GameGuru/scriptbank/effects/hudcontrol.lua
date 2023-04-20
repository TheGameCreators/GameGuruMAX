-- Hud Control v1   by Necrym59
-- DESCRIPTION: Attach to a single-use zone to either hide or show in-game HUDs.
-- DESCRIPTION: [@MODE=1(1=Show, 2=Hide)]
-- DESCRIPTION: [MODE_TEXT$="Hud disabled/enabled"]

	local hudcontrol	= {}
	local hud_mode		= {}
	local mode_text 	= {}
	
function hudcontrol_properties(e, hud_mode, mode_text)
	hudcontrol[e] = {}
	hudcontrol[e].hud_mode = hud_mode
	hudcontrol[e].mode_text = mode_text
end 

function hudcontrol_init(e)
	hudcontrol[e] = {}
	hudcontrol[e].hud_mode = 1
	hudcontrol[e].mode_text = "Hud disabled/enabled"
end

function hudcontrol_main(e)
	if g_Entity[e]['plrinzone']==1 then
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
