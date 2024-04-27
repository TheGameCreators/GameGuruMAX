-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- FuseBox v6 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating other linked objects and/or IfUsed game element. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "E to use"],
-- DESCRIPTION: [USE_RANGE=80(1,100)] 
-- DESCRIPTION: [FAIL_TEXT$ = "Fuse Needed"]
-- DESCRIPTION: [SUCCESS_TEXT$ = "Power Flow Enabled"]
-- DESCRIPTION: Play <Sound0> when placing fuse.
-- DESCRIPTION: Play <Sound1> power enabled.

g_fuses = {}
local fusebox = {}
local prompt_text = {}
local use_range = {}
local fail_text = {}
local success_text = {}
local status = {}
local fuses_required = {}
local fuses_level = {}
local activated = {}
local timer = {}
local played = {}
local doonce = {}
local wait = {}

function fusebox_properties(e, prompt_text, use_range, fail_text, success_text, status, fuses_required)
	fusebox[e].prompt_text = prompt_text
	fusebox[e].use_range = use_range
	fusebox[e].fail_text = fail_text
	fusebox[e].success_text = success_text
	fusebox[e].status = 1
	fusebox[e].fuses_required = 1
end 

function fusebox_init(e)
	fusebox[e] = {}
	fusebox[e].prompt_text = "to use"	
	fusebox[e].use_range = 80
	fusebox[e].fail_text = "Fuse Needed"
	fusebox[e].success_text = "Power Flow Enabled"
	fusebox[e].status = 1
	fusebox[e].fuses_required = 1
	fuses_level[e] = 0	
	activated[e] = 0
	g_fuses = 0
	played[e] = 0
	doonce[e] = 0
	wait[e] = 0
end

function fusebox_main(e)
	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < fusebox[e].use_range then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if fusebox[e].status == 1 then  --Disabled			
			if LookingAt == 1 then
				if doonce[e] == 0 then
					PromptLocal(e,fusebox[e].prompt_text)
					doonce[e] = 1
				end
				if g_KeyPressE == 1 then
					wait[e] = g_Time + 2000
					if g_fuses > 0 then						
						fuses_level[e] = g_fuses
						g_fuses = 0						
					end
					if fuses_level[e] >= fusebox[e].fuses_required then	
						PlaySound(e,0)						
						fusebox[e].status = 2
					else						
						activated[e] = 0
						PromptLocal(e,fusebox[e].fail_text)
						fusebox[e].status = 1
					end					
				end
				if g_KeyPressE == 0 then played[e] = 0 end	
				if g_Time >= wait[e] then doonce[e] = 0 end
			end			
		end
		
		if fusebox[e].status == 2 then --Enabled
			
			if g_KeyPressE == 1 and activated[e] == 0 then
				PromptLocal(e,fusebox[e].success_text)
				SetActivatedWithMP(e,201)
				activated[e] = 1
				PlaySound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				SetObjectFrame(g_Entity[e]['obj'],6) --rem out if different model used				
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end		
	end	
end
