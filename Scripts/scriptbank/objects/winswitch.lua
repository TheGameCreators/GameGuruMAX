-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- WinSwitch v15: by Necrym59 and Lee
-- DESCRIPTION: This object will be treated as a switch object to end the level.
-- DESCRIPTION: Edit the [PROMPT_TEXT$="to End Level"]
-- DESCRIPTION: Set the [USE_RANGE=90(1,200)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\switch.png"]
-- DESCRIPTION: Select [@GoToLevelMode=1(1=Use Storyboard Logic,2=Go to Specific Level)] controls whether the next level in the Storyboard, or another level is loaded after the switch is turned on.
-- DESCRIPTION: [ResetStates!=0] when entering the new level
-- DESCRIPTION: <Sound0> played when the object is switched ON by the player.  

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local winswitch 		= {}
local prompt_text 		= {}
local use_range 		= {}
local prompt_display	= {}
local item_highlight	= {}
local highlight_icon	= {}
local resetstates		= {}

local status 			= {}
local doonce			= {}
local tEnt 				= {}
local selectobj 		= {}
local hl_icon 			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

function winswitch_properties(e, prompt_text, use_range, prompt_display, item_highlight, highlight_icon_imagefile, resetstates)
	winswitch[e].prompt_text = prompt_text or ""
	winswitch[e].use_range = use_range or 90	
	winswitch[e].prompt_display = prompt_display or 1
	winswitch[e].item_highlight = item_highlight or 0
	winswitch[e].highlight_icon = highlight_icon_imagefile
	winswitch[e].resetstates = resetstates or 0
	winswitch[e].initialstate = 0
end 

function winswitch_init(e)	
	winswitch[e] = {}
	winswitch[e].prompt_text = "to End Level"	
	winswitch[e].use_range = 90	
	winswitch[e].prompt_display = 1
	winswitch[e].item_highlight = 0
	winswitch[e].highlight_icon = "imagebank\\icons\\switch.png"
	winswitch[e].resetstates = 0
	winswitch[e].initialstate = 0
	
	doonce[e] = 0
	tEnt[e]	= 0
	g_tEnt = 0
	selectobj[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	status[e] = "init"	
end

function winswitch_main(e)
	
	if status[e] == "init" then
		if winswitch[e].item_highlight == 3 and winswitch[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(winswitch[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(winswitch[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(winswitch[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		if winswitch[e].initialstate == 0 then SetActivatedWithMP(e,101) end
		status[e] = "endinit"
	end	
	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < winswitch[e].use_range and g_PlayerHealth > 0 then
		--pinpoint select object--
		module_misclib.pinpoint(e,winswitch[e].use_range,winswitch[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	
	if PlayerDist < winswitch[e].use_range and tEnt[e] == e then
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
