-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Fuse v4 by Necrym59
-- DESCRIPTION: The attached object will give the player a fuse if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

g_fuses = {}
local fuse = {}
local prompt_text = {}
local pickup_range = {}	
local collected = {}		

function fuse_properties(e, prompt_text, pickup_range)
	fuse[e] = g_Entity[e]
	fuse[e].prompt_text = prompt_text
	fuse[e].pickup_range = pickup_range
end

function fuse_init_name(e)	
	fuse[e] = g_Entity[e]	
	fuse[e].prompt_text = "Collected fuse"
	fuse[e].prompt_text = prompt_text
	fuse[e].pickup_range = pickup_range	
	g_fuses = 0
	collected[e] = 0
end

function fuse_main(e)
	fuse[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)	
	if PlayerDist <= fuse[e].pickup_range and collected[e] == 0 then	
		local LookingAt = GetPlrLookingAtEx(e,1)			
		if LookingAt == 1 then
			PromptLocal(e,"E " ..fuse[e].prompt_text)
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				PromptLocal(e,"Collected")
				PerformLogicConnections(e)
				g_fuses = 1
				collected[e] = 1
				Destroy(e)
			end
		end
	end		
end

