-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- FuseBox v3 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "to use"], [USE_RANGE=80(1,100)] 
-- DESCRIPTION: Play <Sound0> when placing fuse .
-- DESCRIPTION: Play <Sound1> power enabled.

g_fuses = {}
local fusebox = {}
local prompt_text = {}
local use_range = {}
local status = {}
local fuses_required = {}
local fuses_level = {}
local activated = {}
local timer = {}
local played = {}

function fusebox_properties(e, prompt_text, use_range, status, fuses_required)
	fusebox[e] = g_Entity[e]
	fusebox[e].prompt_text = prompt_text
	fusebox[e].use_range = use_range
	fusebox[e].status = 1
	fusebox[e].fuses_required = 1
end 

function fusebox_init(e)
	fusebox[e] = g_Entity[e]
	fusebox[e].prompt_text = "to use"	
	fusebox[e].use_range = 80
	fusebox[e].status = 1
	fusebox[e].fuses_required = 1
	fuses_level[e] = 0	
	activated[e] = 0
	g_fuses = g_fuses
	played = 0		
end

function fusebox_main(e)
	fusebox[e] = g_Entity[e]	
	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < fusebox[e].use_range and g_PlayerHealth > 0 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if fusebox[e].status == 1 then  --Disabled		
			if LookingAt == 1 then				
				if GetGamePlayerStateXBOX() == 1 then
					PromptLocalForVR(e,"Y Button " ..fusebox[e].prompt_text,3)
				else
					if GetHeadTracker() == 1 then
						PromptLocalForVR(e,"Trigger " ..fusebox[e].prompt_text,3)
					else
						PromptLocalForVR(e,"E " ..fusebox[e].prompt_text,3)
					end					
				end
				if g_KeyPressE == 1 then
					if g_fuses > 0 then						
						fuses_level[e] = g_fuses
						g_fuses = 0						
					end
					if fuses_level[e] >= fusebox[e].fuses_required then	
						PlaySound(e,0)						
						fusebox[e].status = 2
					else						
						activated[e] = 0
						Prompt("Fuse Needed")
						fusebox[e].status = 1
					end
				end
				if g_KeyPressE == 0 then played = 0 end					
			end			
		end
		
		if fusebox[e].status == 2 then --Enabled
			
			if g_KeyPressE == 1 and activated[e] == 0 then
				PromptDuration("Power Flow Enabled",2000)
				SetActivatedWithMP(e,201)
				activated[e] = 1
				PlaySound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				SetObjectFrame(g_Entity[e]['obj'],6) --rem out if different model used				
				PerformLogicConnections(e)
			end			
		end		
	end	
end
