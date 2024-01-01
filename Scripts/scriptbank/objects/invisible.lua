-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Invisible v2
-- DESCRIPTION: Hides an object for the duration of play (collision on or off)
-- DESCRIPTION: Use [PROMPT_TEXT$=""] or leave blank to use Object name, [@PROMPT_MODE=1(1=On, 2=Off)]
-- DESCRIPTION: Prompt if on will be shown when the player is within [RANGE=100(60,3000)]
-- DESCRIPTION: Object [@COLLISION_MODE=1(1=On, 2=Off)] on or off.

	local invisible = {}
	local prompt_text = {}
	local prompt_mode = {}
	local range = {}
	local collision_mode = {}
	local iname = {}

function invisible_init_name(e,name)
	invisible[e] = g_Entity[e]
	iname[e] = name
	invisible[e].prompt_text = ""
	invisible[e].range = 100
	invisible[e].collision_mode = 1
end

function invisible_properties(e, prompt_text, prompt_mode, range, collision_mode)
	invisible[e] = g_Entity[e]
	invisible[e].prompt_text = prompt_text
	invisible[e].prompt_mode = prompt_mode
	invisible[e].range = range
	invisible[e].collision_mode = collision_mode
end 

function invisible_main(e)
	invisible[e] = g_Entity[e]
	if invisible[e].prompt_text == "" then invisible[e].prompt_text = iname[e] end
	Hide(e)
	local PlayerDist = GetPlayerDistance(e)	
	if PlayerDist < invisible[e].range and g_PlayerHealth > 0 then
		if invisible[e].prompt_mode == 1 then
			PromptLocal (e, invisible[e].prompt_text)
		end	
	end
	if invisible[e].collision_mode == 2 then
		CollisionOff(e)
	end		
end
