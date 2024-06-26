-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- FuseBox v7 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating other linked objects and/or IfUsed game element. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "E to use"],
-- DESCRIPTION: [USE_RANGE=80(1,100)] 
-- DESCRIPTION: [FAIL_TEXT$ = "Fuse Needed"]
-- DESCRIPTION: [SUCCESS_TEXT$ = "Power Flow Enabled"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
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
local animonce = {}
local wait = {}
local tusedvalue = {}
local use_item_now = {}

function fusebox_properties(e, prompt_text, use_range, fail_text, success_text, prompt_display)
	fusebox[e].prompt_text = prompt_text
	fusebox[e].use_range = use_range
	fusebox[e].fail_text = fail_text
	fusebox[e].success_text = success_text
	fusebox[e].prompt_display = prompt_display
	fusebox[e].status = 1
	fusebox[e].fuses_required = 1
end 

function fusebox_init(e)
	fusebox[e] = {}
	fusebox[e].prompt_text = "to use"	
	fusebox[e].use_range = 80
	fusebox[e].fail_text = "Fuse Needed"
	fusebox[e].success_text = "Power Flow Enabled"
	fusebox[e].prompt_display = 1	
	fusebox[e].status = 1
	fusebox[e].fuses_required = 1
	
	fuses_level[e] = 0	
	activated[e] = 0
	g_fuses = g_fuses
	played[e] = 0
	doonce[e] = 0
	animonce[e] = 0
	wait[e] = 0
	tusedvalue[e] = 0
	use_item_now[e] = 0
end

function fusebox_main(e)
	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < fusebox[e].use_range then
		local LookingAt = GetPlrLookingAtEx(e,1)
		
		if fusebox[e].status == 1 then  --Disabled
			if animonce[e] == 0 then
				SetAnimationName(e,"Off")
				PlayAnimation(e)
				animonce[e] = 1
			end	
			if LookingAt == 1 then
				if doonce[e] == 0 then
					if fusebox[e].prompt_display == 1 then PromptLocal(e,fusebox[e].prompt_text) end
					if fusebox[e].prompt_display == 2 then Prompt(fusebox[e].prompt_text) end
					doonce[e] = 1
				end				
				if g_KeyPressE == 1 then
					wait[e] = g_Time + 2000
					if g_fuses > 0 then
						tusedvalue[e] = g_fuses
						fuses_level[e] = 1
					end
					if fuses_level[e] >= fusebox[e].fuses_required then	
						PlaySound(e,0)						
						fusebox[e].status = 2
					else						
						activated[e] = 0
						if fusebox[e].prompt_display == 1 then PromptLocal(e,fusebox[e].fail_text) end
						if fusebox[e].prompt_display == 2 then Prompt(fusebox[e].fail_text) end
						fusebox[e].status = 1
					end					
				end
				if g_KeyPressE == 0 then played[e] = 0 end	
				if g_Time >= wait[e] then doonce[e] = 0 end
			end			
		end
		
		if fusebox[e].status == 2 then --Enabled			
			if g_KeyPressE == 1 and activated[e] == 0 then
				if fusebox[e].prompt_display == 1 then PromptLocal(e,fusebox[e].success_text) end
				if fusebox[e].prompt_display == 2 then Prompt(fusebox[e].success_text) end			
				SetActivatedWithMP(e,201)
				activated[e] = 1
				PlaySound(e,1)
				SetAnimationName(e,"On")
				SetAnimationSpeed(e,1)
				PlayAnimation(e)
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				if tusedvalue[e] > 0 then
					-- if this is a resource, it will deplete qty and set used to zero
					SetEntityUsed(e,tusedvalue[e]*-1)
					use_item_now[e] = 1
				end
				if use_item_now[e] == 1 then
					Destroy(g_fuses) -- can only destroy resources that are qty zero
					g_fuses = 0
				end
				SwitchScript(e,"no_behavior_selected.lua")
			end
		end		
	end	
end
