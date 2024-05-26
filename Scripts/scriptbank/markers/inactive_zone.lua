-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Inactive Zone v3   by Necrym59
-- DESCRIPTION: An inactive zone, when triggered will activate all linked entities to the zone and removes the zone afterwards.
-- DESCRIPTION: Can be triggered by another zone or switch.
-- DESCRIPTION: Enter the [PROMPT_TEXT$="Zone has been activated"]
-- DESCRIPTION: Use <Sound0> when zone activated

local inactivezone 		= {}
local prompt_text 		= {}
local status			= {}

function inactive_zone_properties(e, prompt_text)
	inactivezone[e] = g_Entity[e]
	inactivezone[e].prompt_text = prompt_text
end

function inactive_zone_init(e)
	inactivezone[e] = g_Entity[e]
	inactivezone[e].prompt_text = "Zone has been activated"
	status[e] = "init"
end

function inactive_zone_main(e)
	inactivezone[e] = g_Entity[e]

	if status[e] == "init" then
		SetActivated(e,0)
		status[e] = "endinit"
	end

	if g_Entity[e]['activated'] == 1 then
		PerformLogicConnections(e)
		Destroy(e)
	end
end

function inactive_zone_exit(e)
end
