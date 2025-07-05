-- Generator v14
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [@FUEL_STATE=1(1=Empty, 2=Full)]
-- DESCRIPTION: [FUEL_REQUIRED=20(1,100)]
-- DESCRIPTION: [FAIL_TEXT$="More fuel needed"]
-- DESCRIPTION: [RUNNING_TEXT$="Generator Running, Q to stop"]
-- DESCRIPTION: [STOPPED_TEXT$="Generator Stopped"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: Play <Sound0> when starting.
-- DESCRIPTION: Play <Sound1> running loop.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_fuel = {}
g_fuelamount = {}
g_fuelcollected = {}

local generator			= {}
local prompt_text		= {}
local use_range			= {}
local fuel_state		= {}
local fuel_required		= {}
local fail_text			= {}
local running_text		= {}
local stopped_text		= {}
local prompt_display	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local status		= {}
local tEnt 			= {}
local fuel_level	= {}
local running		= {}
local timer			= {}
local played		= {}
local tusedvalue	= {}
local use_item_now	= {}
local hl_icon		= {}
local hl_imgwidth	= {}
local hl_imgheight	= {}

function generator_properties(e, prompt_text, use_range, fuel_state, fuel_required, fail_text, running_text, stopped_text, prompt_display, item_highlight, highlight_icon_imagefile)
	generator[e].prompt_text = prompt_text
	generator[e].use_range = use_range
	generator[e].fuel_state = fuel_state
	generator[e].fuel_required = fuel_required
	generator[e].fail_text = fail_text
	generator[e].running_text = running_text
	generator[e].stopped_text = stopped_text	
	generator[e].prompt_display = prompt_display
	generator[e].item_highlight = item_highlight
	generator[e].highlight_icon = highlight_icon_imagefile	
end 

function generator_init(e)
	generator[e] = {}
	generator[e].prompt_text = "to use"	
	generator[e].use_range = 80
	generator[e].fuel_state = 1
	generator[e].fuel_required = 20
	generator[e].fail_text = "More fuel needed"
	generator[e].running_text = "Generator Running"
	generator[e].stopped_text = "Generator Stopped"
	generator[e].prompt_display = 1
	generator[e].item_highlight = 0
	generator[e].highlight_icon = highlight_icon_imagefile	
	
	fuel_level[e] = 0	
	running[e] = 0
	timer[e] = 0
	g_fuel = 0
	g_fuelamount = g_fuelamount
	played = 0
	g_tEnt = 0
	tEnt[e] = 0
	tusedvalue[e] = 0
	use_item_now[e] = 0	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function generator_main(e)

	if status[e] == "init" then
		if generator[e].item_highlight == 3 and generator[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(generator[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(generator[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(generator[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < generator[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,generator[e].use_range,generator[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end
	
	if PlayerDist < generator[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then	
		if generator[e].fuel_state == 1 then  --Empty
			if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].prompt_text) end
			if generator[e].prompt_display == 2 then Prompt(generator[e].prompt_text) end	
			if g_KeyPressE == 1 then
				if g_fuel > 0 then
					tusedvalue[e] = g_fuel
					fuel_level[e] = (fuel_level[e] + g_fuelamount)
					g_fuelamount = 0
					g_fuelcollected = 0
				end
				if fuel_level[e] >= generator[e].fuel_required then
					PlaySound(e,0)						
					generator[e].fuel_state = 2
				else						
					if played == 0 then
						PlaySound(e,0)
						played = 1
					end
					running[e] = 0
					if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].fail_text) end
					if generator[e].prompt_display == 2 then Prompt(generator[e].fail_text) end							
					generator[e].fuel_state = 1
				end
			end
			if g_KeyPressE == 0 then played = 0 end
		end
		
		if generator[e].fuel_state == 2 then --Full
			if running[e] == 0 then
				if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].prompt_text) end
				if generator[e].prompt_display == 2 then Prompt(generator[e].prompt_text) end	
			end
			if running[e] == 1 then
				if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].running_text) end
				if generator[e].prompt_display == 2 then Prompt(generator[e].running_text) end	
			end
			if g_KeyPressE == 1 and running[e] == 0 then
				SetActivatedWithMP(e,201)
				running[e] = 1
				LoopSound(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)
				PerformLogicConnections(e)				
			end			
			if g_KeyPressQ == 1 and running[e] == 1 then 
				SetActivatedWithMP(e,101)
				running[e] = 0
				if generator[e].prompt_display == 1 then PromptLocal(e,generator[e].stopped_text) end
				if generator[e].prompt_display == 2 then Prompt(generator[e].stopped_text) end
				StopSound(e,1)
				SetAnimationName(e,"off")
				PlayAnimation(e)
				PerformLogicConnections(e)
			end			
		end
		
		if tusedvalue[e] > 0 then
			-- if this is a resource, it will deplete qty and set used to zero
			SetEntityUsed(e,tusedvalue[e]*-1)
			use_item_now[e] = 1			
		end
		
		if use_item_now[e] == 1 then
			Destroy(g_fuel) -- can only destroy resources that are qty zero
		end
	end	
end
