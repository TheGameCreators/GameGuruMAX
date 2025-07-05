-- Fuel v12
-- DESCRIPTION: The attached object will give the player a fuel resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [AMOUNT=5(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [COLLECTED_TEXT$="Collected Fuel"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

g_fuel = {}
g_fuelamount = {}
g_fuelcollected = {}

local fuel 				= {}
local prompt_text 		= {}
local amount 			= {}
local pickup_range 		= {}
local pickup_style 		= {}
local collected_text 	= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon	= {}

local total 			= {}
local tEnt 				= {}
local selectobj 		= {}
local collected			= {}
local collectonce		= {}
local doonce			= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}
local status			= {}

function fuel_properties(e, prompt_text, amount, pickup_range, pickup_style, collected_text, prompt_display, item_highlight, highlight_icon_imagefile)
	fuel[e].prompt_text = prompt_text
	fuel[e].amount = amount
	fuel[e].pickup_range = pickup_range
	fuel[e].pickup_style = pickup_style
	fuel[e].collected_text = collected_text
	fuel[e].prompt_display = prompt_display
	fuel[e].item_highlight = item_highlight
	fuel[e].highlight_icon = highlight_icon_imagefile
end

function fuel_init_name(e)
	fuel[e] = {}
	fuel[e].prompt_text = "E to Collect"
	fuel[e].amount = 5
	fuel[e].pickup_range = 80
	fuel[e].pickup_style = 1
	fuel[e].collected_text = "Collected Fuel"
	fuel[e].prompt_display = 1
	fuel[e].item_highlight = 0
	fuel[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	g_fuel = 0
	g_fuelamount = 0
	g_fuelcollected = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	collected[e] = 0
	doonce[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function fuel_main(e)

	if status[e] == "init" then
		if fuel[e].item_highlight == 3 and fuel[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(fuel[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(fuel[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(fuel[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < fuel[e].pickup_range and g_fuelcollected == 1 then
		if fuel[e].prompt_display == 1 then PromptLocal(e,"Only one fuel cannister can be carried at a time") end
		if fuel[e].prompt_display == 2 then Prompt("Only one fuel cannister can be carried at a time") end
	end

	if fuel[e].pickup_style == 1 then
		if PlayerDist < fuel[e].pickup_range and collected[e] == 0 and g_fuelcollected == 0 then
			if GetEntityCollectable(e) == 0 then
				if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].collected_text) end
				if fuel[e].prompt_display == 2 then Prompt(fuel[e].collected_text) end
				PlaySound(e,0)
				PerformLogicConnections(e)
				g_fuelamount = g_fuelamount+fuel[e].amount
				collected[e] = 1
				Hide(e)
				CollisionOff(e)
				Destroy(e)
			end
			if GetEntityCollectable(e) == 1 or GetEntityCollectable(e) == 2 then
				if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].collected_text) end
				if fuel[e].prompt_display == 2 then Prompt(fuel[e].collected_text) end
				PlaySound(e,0)
				PerformLogicConnections(e)
				SetEntityCollected(e,1)
				g_fuel = e
				g_fuelamount = g_fuelamount+fuel[e].amount
				g_fuelcollected = 1
				Hide(e)
				CollisionOff(e)
				collected[e] = 1
			end
		end		
	end

	if fuel[e].pickup_style == 2 then
		if PlayerDist < fuel[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,fuel[e].pickup_range,fuel[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < fuel[e].pickup_range and collected[e] == 0 and tEnt[e] == e and g_fuelcollected == 0 then
			if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].prompt_text) end
			if fuel[e].prompt_display == 2 then Prompt(fuel[e].prompt_text) end			
			if g_KeyPressE == 1 then
				if GetEntityCollectable(tEnt[e]) == 0 then
					PlaySound(e,0)
					PerformLogicConnections(e)
					if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].collected_text) end
					if fuel[e].prompt_display == 2 then Prompt(fuel[e].collected_text) end
					collected[e] = 1
					g_fuel = e					
					g_fuelamount = g_fuelamount + fuel[e].amount
					g_fuelcollected = 1	
					Hide(e)
					CollisionOff(e)
					Destroy(e)
				end
				if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
					PlaySound(e,0)
					PerformLogicConnections(e)
					if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].collected_text) end
					if fuel[e].prompt_display == 2 then Prompt(fuel[e].collected_text) end
					SetEntityCollected(tEnt[e],1)
					collected[e] = 1
					g_fuel = e
					g_fuelamount = g_fuelamount + fuel[e].amount
					g_fuelcollected = 1					
					Hide(e)					
 					CollisionOff(e)				
				end				
			end	
		end
	end
end
