-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinObject v3: by Necrym59
-- DESCRIPTION: Will win the level by obtaining this object? Set Always Active ON.
-- DESCRIPTION: [PROMPT_TEXT$="E to pickup"]
-- DESCRIPTION: [USEAGE_TEXT$="X to use"]
-- DESCRIPTION: [USE_RANGE=90(1,200)]
-- DESCRIPTION: Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after object use.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> for pickup
-- DESCRIPTION: <Sound1> on use

local winobject 		= {}
local prompt_text 		= {}
local useage_text 		= {}
local use_range 		= {}
local have_winobject 	= {}
local resetstates		= {}
local doonce			= {}
	
function winobject_properties(e, prompt_text, useage_text, use_range, resetstates)
	winobject[e].prompt_text = prompt_text
	winobject[e].useage_text = useage_text
	winobject[e].use_range = use_range
	winobject[e].resetstates = resetstates
end 	
	
function winobject_init(e)
	winobject[e] = {}	
	winobject[e].prompt_text = "E to pickup"
	winobject[e].useage_text = "X to use"
	winobject[e].use_range = 90
	winobject[e].resetstates = 0
	have_winobject = 0
	doonce[e] = 0
end
 
function winobject_main(e)
	winobject[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if have_winobject == 0 then
		if PlayerDist < winobject[e].use_range and have_winobject == 0 then
			local LookingAt = GetPlrLookingAtEx(e,1)	
			if LookingAt == 1 then		
				Prompt(winobject[e].prompt_text)
				if g_KeyPressE == 1 then
					have_winobject = 1					
					PromptDuration(winobject[e].useage_text,2000)
					if doonce[e] == 0 then 
						PlaySound(e,1)
						doonce[e] = 1
					end
					PlaySound(e,0)
					Hide(e)
					CollisionOff(e)					
				end
			end
		end
	end
		
	if have_winobject == 1 then
		if GetInKey() == "x" or GetInKey() == "X" then
			if doonce[e] == 1 then 
				PlaySound(e,1)
				doonce[e] = 2
			end
			PlaySound(e,1)
			PerformLogicConnections(e)
			ActivateIfUsed(e)
			JumpToLevelIfUsedEx(e,winobject[e].resetstates)
			Destroy(e)
		end	
	end
end

function winitem_init(e)
end

