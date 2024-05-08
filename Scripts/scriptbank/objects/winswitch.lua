-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinSwitch v14: by Necrym59 and Lee
-- DESCRIPTION: This object will be treated as a switch object to end the level.
-- DESCRIPTION: Edit the [PROMPT_TEXT$="to End Level"]
-- DESCRIPTION: Set the [USE_RANGE=90(1,200)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after the switch is turned on.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> played when the object is switched ON by the player.  

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local winswitch 		= {}
local prompt_text 		= {}
local use_range 		= {}
local item_highlight	= {}
local resetstates		= {}

local doonce			= {}
local tEnt 				= {}
local selectobj 		= {}

function winswitch_properties(e, prompt_text, use_range, item_highlight, resetstates)
	winswitch[e].prompt_text = prompt_text or ""
	winswitch[e].use_range = use_range or 90	
	winswitch[e].item_highlight = item_highlight or 0
	winswitch[e].resetstates = resetstates or 0
	winswitch[e].initialstate = 0
end 

function winswitch_init(e)	
	winswitch[e] = {}
	winswitch[e].prompt_text = "to End Level"	
	winswitch[e].use_range = 90	
	winswitch[e].item_highlight = 0
	winswitch[e].resetstates = 0
	winswitch[e].initialstate = 0
	doonce[e] = 0
	tEnt[e]	= 0
	g_tEnt = 0
	selectobj[e] = 0
end

function winswitch_main(e)	
	local PlayerDist = GetPlayerDistance(e)
	if winswitch[e].initialstate == 0 then SetActivatedWithMP(e,101) end
	
	if PlayerDist < winswitch[e].use_range and g_PlayerHealth > 0 then
		--pinpoint select object--
		module_misclib.pinpoint(e,winswitch[e].use_range,winswitch[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	
	if PlayerDist < winswitch[e].use_range and tEnt[e] ~= 0 then
		if g_Entity[e].activated == 101 then
			if GetGamePlayerStateXBOX() == 1 then
				PromptLocalForVR(e,"Y Button " .. winswitch[e].prompt_text,3)
			else
				if GetHeadTracker() == 1 then
					PromptLocalForVR(e,"Trigger " ..winswitch[e].prompt_text,3)
				else
					PromptLocalForVR(e,"E " .. winswitch[e].prompt_text,3)
				end
			end
			if g_KeyPressE == 1 then
				SetActivatedWithMP(e,201)
				SetAnimationName(e,"on")
				PlayAnimation(e) 
				PerformLogicConnections(e)
				if doonce[e] == 0 then 
					PlaySound(e,0)
					doonce[e] = 10
				end
				JumpToLevelIfUsedEx(e,winswitch[e].resetstates)
			end			
		end
	end
end
