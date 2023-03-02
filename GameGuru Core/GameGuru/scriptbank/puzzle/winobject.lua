-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinObject v2: by Necrym59
-- DESCRIPTION: Will win the level by obtaining this object? Set Always Active ON.
-- DESCRIPTION: [PROMPT_TEXT$="E to pickup"]
-- DESCRIPTION: [USEAGE_TEXT$="X to use"]
-- DESCRIPTION: Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after object use.
-- DESCRIPTION: <Sound0> for pickup
-- DESCRIPTION: <Sound1> on use

local winobject 		= {}
local prompt_text 		= {}
local have_winobject 	= {}
	
function winobject_properties(e, prompt_text, useage_text)
	winobject[e] = g_Entity[e]
	winobject[e].prompt_text = prompt_text
	winobject[e].useage_text = useage_text
end 	
	
function winobject_init(e)
	winobject[e] = g_Entity[e]	
	winobject[e].prompt_text = "E to pickup"
	winobject[e].useage_text = "F to use"
	have_winobject = 0
end
 
function winobject_main(e)
	winobject[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	
	if have_winobject == 0 then
		if PlayerDist < 80 and g_PlayerHealth > 0 and have_winobject == 0 then
			local LookingAt = GetPlrLookingAtEx(e,1)	
			if LookingAt == 1 then		
				Prompt(winobject[e].prompt_text)
				if g_KeyPressE == 1 then
					have_winobject = 1					
					PromptDuration(winobject[e].useage_text,2000)
					PlaySound(e,0)
					Hide(e)
					CollisionOff(e)					
				end
			end
		end
	end
		
	if have_winobject == 1 then
		if GetInKey() == "x" or GetInKey() == "X" then
			PlaySound(e,1)
			JumpToLevelIfUsed(e)
			ActivateIfUsed(e)
			Destroy(e)
		end	
	end
end

function winitem_init(e)
end

