-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinObject v5: by Necrym59
-- DESCRIPTION: Will win the level by obtaining this object? Set Always Active ON.
-- DESCRIPTION: [PROMPT_TEXT$="E to pickup"]
-- DESCRIPTION: [USEAGE_TEXT$="X to use"]
-- DESCRIPTION: [PICKUP_RANGE=90(1,200)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after object use.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> for pickup
-- DESCRIPTION: <Sound1> on use

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local winobject 		= {}
local prompt_text 		= {}
local useage_text 		= {}
local pickup_range 		= {}
local resetstates		= {}
local prompt_display 	= {}
local item_highlight 	= {}

local doonce			= {}
local have_winobject 	= {}
local tEnt 				= {}
local selectobj 		= {}
	
function winobject_properties(e, prompt_text, useage_text, pickup_range, prompt_display, item_highlight, resetstates)
	winobject[e].prompt_text = prompt_text
	winobject[e].useage_text = useage_text
	winobject[e].pickup_range = pickup_range
	winobject[e].prompt_display = prompt_display
	winobject[e].item_highlight = item_highlight or 0	
	winobject[e].resetstates = resetstates or 0	
end 	
	
function winobject_init(e)
	winobject[e] = {}	
	winobject[e].prompt_text = "E to pickup"
	winobject[e].useage_text = "X to use"
	winobject[e].pickup_range = 90
	winobject[e].prompt_display = 1
	winobject[e].item_highlight = 0
	winobject[e].resetstates = 0	
	
	have_winobject[e] = 0
	doonce[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
end
 
function winobject_main(e)

	local PlayerDist = GetPlayerDistance(e)
	
	if have_winobject[e] == 0 then
		if PlayerDist < winobject[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e, winobject[e].pickup_range, winobject[e].item_highlight)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
			if PlayerDist < winobject[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
				if winobject[e].prompt_display == 1 then PromptLocal(e,winobject[e].prompt_text) end
				if winobject[e].prompt_display == 2 then Prompt(winobject[e].prompt_text) end			
				if g_KeyPressE == 1 then					
					if winobject[e].prompt_display == 1 then PromptLocal(e,winobject[e].useage_text) end
					if winobject[e].prompt_display == 2 then PromptDuration(winobject[e].useage_text,2000) end						
					if doonce[e] == 0 then 
						PlaySound(e,0)
						doonce[e] = 1						
					end
					Hide(e)
					CollisionOff(e)
					have_winobject[e] = 1					
				end
			end
		end
	end
		
	if have_winobject[e] == 1 then
		if GetInKey() == "u" or GetInKey() == "U" then
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

