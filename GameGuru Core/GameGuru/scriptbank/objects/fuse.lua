-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Fuse v3 by Necrym59
-- DESCRIPTION: The attached object will give the player a fuse if collected.
-- DESCRIPTION: [PROMPT_TEXT$="to collect"]
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
	if PlayerDist <= fuse[e].pickup_range and g_PlayerHealth > 0 and collected[e] == 0 then	
		local LookingAt = GetPlrLookingAtEx(e,1)			
		if LookingAt == 1 then
			if GetGamePlayerStateXBOX() == 1 then
				PromptLocalForVR(e,"Y Button " ..fuse[e].prompt_text,3)
			else
				if GetHeadTracker() == 1 then
					PromptLocalForVR(e,"Trigger " ..fuse[e].prompt_text,3)
				else
					PromptLocalForVR(e,"E " ..fuse[e].prompt_text,3)
				end					
			end			
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				PromptLocalForVR(e,"Collected",3)
				PerformLogicConnections(e)
				g_fuses = 1
				collected[e] = 1
				Destroy(e)
			end
		end
	end		
end

