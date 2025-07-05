-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- FuseBox v8 by Necrym59
-- DESCRIPTION: This object will be treated as a switch object for activating other linked objects and/or IfUsed game element. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$ = "E to use"],
-- DESCRIPTION: [USE_RANGE=80(1,100)] 
-- DESCRIPTION: [FAIL_TEXT$ = "Fuse Needed"]
-- DESCRIPTION: [SUCCESS_TEXT$ = "Power Flow Enabled"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> when placing fuse.
-- DESCRIPTION: <Sound1> fail attempt.
-- DESCRIPTION: <Sound2> power enabled.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_fuses = {}
local fusebox 		= {}
local prompt_text	= {}
local use_range		= {}
local fail_text		= {}
local success_text	= {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}
local fuses_required = {}

local status = {}
local tEnt = {}
local fuses_level = {}
local activated = {}
local timer = {}
local played = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local tusedvalue = {}
local use_item_now = {}

function fusebox_properties(e, prompt_text, use_range, fail_text, success_text, prompt_display, item_highlight, highlight_icon_imagefile)
	fusebox[e].prompt_text = prompt_text
	fusebox[e].use_range = use_range
	fusebox[e].fail_text = fail_text
	fusebox[e].success_text = success_text
	fusebox[e].prompt_display = prompt_display
	fusebox[e].item_highlight = item_highlight
	fusebox[e].highlight_icon = highlight_icon_imagefile	
end 

function fusebox_init(e)
	fusebox[e] = {}
	fusebox[e].prompt_text = "to use"	
	fusebox[e].use_range = 80
	fusebox[e].fail_text = "Fuse Needed"
	fusebox[e].success_text = "Power Flow Enabled"
	fusebox[e].prompt_display = 1
	fusebox[e].item_highlight = 0
	fusebox[e].highlight_icon = "imagebank\\icons\\hand.png"
	fusebox[e].fuses_required = 1	
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	fuses_level[e] = 0	
	activated[e] = 0
	g_fuses = g_fuses
	played[e] = 0
	g_tEnt = 0
	tEnt[e] = 0	
	tusedvalue[e] = 0
	use_item_now[e] = 0
end

function fusebox_main(e)

	if status[e] == "init" then
		if fusebox[e].item_highlight == 3 and fusebox[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(fusebox[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(fusebox[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(fusebox[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "disabled"
	end	
	
	local PlayerDist = GetPlayerDistance(e)
		
	if PlayerDist < fusebox[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,fusebox[e].use_range,fusebox[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	if PlayerDist < fusebox[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
		if status[e] == "disabled" then  --Disabled
			if fusebox[e].prompt_display == 1 then PromptLocal(e,fusebox[e].prompt_text) end
			if fusebox[e].prompt_display == 2 then Prompt(fusebox[e].prompt_text) end			
			if g_KeyPressE == 1 then
				if g_fuses > 0 then
					tusedvalue[e] = g_fuses
					fuses_level[e] = 1
				end
				if fuses_level[e] >= fusebox[e].fuses_required then	
					PlaySound(e,0)						
					status[e] = "enabled"
				else						
					activated[e] = 0
					if fusebox[e].prompt_display == 1 then PromptLocal(e,fusebox[e].fail_text) end
					if fusebox[e].prompt_display == 2 then Prompt(fusebox[e].fail_text) end
					PlaySound(e,1)
					status[e] = "disabled"
				end					
			end
			if g_KeyPressE == 0 then played[e] = 0 end	
		end			
		
		if status[e] == "enabled" then --Enabled			
			if g_KeyPressE == 1 and activated[e] == 0 then
				if fusebox[e].prompt_display == 1 then PromptLocal(e,fusebox[e].success_text) end
				if fusebox[e].prompt_display == 2 then Prompt(fusebox[e].success_text) end			
				SetActivatedWithMP(e,201)
				activated[e] = 1
				PlaySound(e,2)
				SetAnimationName(e,"On")				
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
	if status[e] == "disabled" then
		SetAnimationName(e,"Off")
		PlayAnimation(e)
	end
	if status[e] == "enabled" then
		SetAnimationName(e,"On")
		PlayAnimation(e)
	end
end
