-- Pipevalve v7 by Necrym59 
-- DESCRIPTION: This object will be treated as a switch object for activating other linked objects and/or IfUsed game element. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [USE_RANGE=80(1,150)]
-- DESCRIPTION: [FAIL_TEXT$="Valve wheel required"]
-- DESCRIPTION: [SUCCESS_TEXT$="Valve opened"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> when attaching.
-- DESCRIPTION: <Sound1> when turning
-- DESCRIPTION: <Sound2> when opened

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_valve = {}

local pipevalve			= {}
local prompt_text		= {}
local use_range			= {}
local fail_text			= {}
local success_text 		= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local tEnt = {}
local have_valve = {}
local activated = {}
local tusedvalue = {}
local use_item_now = {}
local pressed = {}

function pipevalve_properties(e, prompt_text, use_range, fail_text, success_text, prompt_display, item_highlight, highlight_icon_imagefile)
	pipevalve[e].prompt_text = prompt_text
	pipevalve[e].use_range = use_range
	pipevalve[e].fail_text = fail_text		
	pipevalve[e].success_text = success_text	
	pipevalve[e].prompt_display = prompt_display or 1
	pipevalve[e].item_highlight = item_highlight
	pipevalve[e].highlight_icon = highlight_icon_imagefile		
end 

function pipevalve_init(e)
	pipevalve[e] = {}
	pipevalve[e].prompt_text = "E to use"	
	pipevalve[e].use_range = 80
	pipevalve[e].fail_text = "Valve wheel required"		
	pipevalve[e].success_text = "Valve Opened"	
	pipevalve[e].prompt_display = 1
	pipevalve[e].item_highlight = 0
	pipevalve[e].highlight_icon = "imagebank\\icons\\hand.png"	
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	have_valve[e] = 0	
	activated[e] = 0
	tusedvalue[e] = 0
	use_item_now[e] = 0
	g_valve = 0
	g_tEnt = 0
	tEnt[e] = 0	
	pressed[e] = 0
end

function pipevalve_main(e)

	if status[e] == "init" then
		if pipevalve[e].item_highlight == 3 and pipevalve[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(pipevalve[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(pipevalve[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(pipevalve[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "inactive"
	end
	
	local PlayerDist = GetPlayerDistance(e)	
	
	if PlayerDist < pipevalve[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,pipevalve[e].use_range,pipevalve[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	
	if PlayerDist < pipevalve[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
		if status[e] == "inactive" then  --Inactive	
			if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].prompt_text) end
			if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].prompt_text) end
			if g_KeyPressE == 1 then
				if g_valve > 0 then
					tusedvalue[e] = g_valve
					have_valve[e] = 1
				end
				if have_valve[e] == 1 then					
					SetAnimationName(e,"off")
					SetAnimationSpeed(e,1)					
					PlayAnimation(e)
					PlaySound(e,0)
					activated[e] = 0
					status[e] = "active"				
				end
				if have_valve[e] ~= 1 then	
					activated[e] = 0
					if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].fail_text) end
					if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].fail_text) end
					SetAnimationName(e,"idle")
					PlayAnimation(e)
					status[e] = "inactive"
				end
				if g_KeyPressE == 0 then pressed[e] = 0 end	
			end
		end	
		
		if status[e] == "active" then --Active
			if g_KeyPressE == 1 and activated[e] == 0 then
				if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].success_text) end
				if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].success_text) end				
				SetActivatedWithMP(e,201)				
				activated[e] = 1
				StopSound(e,0)
				PlaySound(e,1)				
				SetAnimationName(e,"on")
				SetAnimationSpeed(e,1)				
				PlayAnimation(e)				
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				PlaySound(e,2)				
				if tusedvalue[e] > 0 then
					-- if this is a resource, it will deplete qty and set used to zero
					SetEntityUsed(e,tusedvalue[e]*-1)
					use_item_now[e] = 1
				end
				if use_item_now[e] == 1 then
					Destroy(g_valve) -- can only destroy resources that are qty zero
					g_valve = 0
				end
			end
		end
	end
end
