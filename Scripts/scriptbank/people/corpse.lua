-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Corpse v1 by Necrym59
-- DESCRIPTION: Allows an npc to be a corpse. Attach to a character.

local corpse = {}

function corpse_properties(e)
	corpse[e] = g_Entity[e]
end

function corpse_init(e)
	corpse[e] = {}
end

function corpse_main(e)
	SetEntityHealth(e,0)
	SwitchScript(e,"no_behavior_selected.lua")
end

