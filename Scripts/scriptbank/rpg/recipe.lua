-- Recipe v7 by Necrym59 and Lee
-- DESCRIPTION: This object will give the player a crafting recipe. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Crafting Recipe collected"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for collected sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local recipe			= {}
local pickup_text		= {}
local collected_text	= {}
local pickup_range		= {}
local pickup_style		= {}
local prompt_display	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local tEnt = {}
local selectobj = {}
local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function recipe_properties(e, pickup_text, collected_text, pickup_range, pickup_style, prompt_display, item_highlight, highlight_icon_imagefile)
	recipe[e].pickup_text = pickup_text
	recipe[e].collected_text = collected_text
	recipe[e].pickup_range = pickup_range
	recipe[e].pickup_style = pickup_style
	recipe[e].prompt_display = prompt_display
	recipe[e].item_highlight = item_highlight
	recipe[e].highlight_icon = highlight_icon_imagefile	
end

function recipe_init(e)
	recipe[e] = {}
	recipe[e].pickup_text = "E to collect"
	recipe[e].collected_text = "Crafting Recipe collected"
	recipe[e].pickup_range = 80
	recipe[e].pickup_style = 2
	recipe[e].prompt_display = 1
	recipe[e].item_highlight = 0
	recipe[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function recipe_main(e)
	
	if status[e] == "init" then
		if recipe[e].item_highlight == 3 and recipe[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(recipe[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(recipe[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(recipe[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if recipe[e].pickup_style == 1 and PlayerDist < recipe[e].pickup_range then
		if GetEntityCollectable(e) == 1 then
			if GetEntityCollected(e) == 0 then	
				PromptDuration(recipe[e].collected_text,2000)				
				PerformLogicConnections(e)
				PlayNon3DSound(e,0)
				SetEntityCollected(e,1,-1)			
			end
		end
	end
	
	if recipe[e].pickup_style == 2 and PlayerDist < recipe[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,recipe[e].pickup_range,recipe[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--		
		if PlayerDist < recipe[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					if recipe[e].prompt_display == 1 then PromptLocal(e,recipe[e].pickup_text) end
					if recipe[e].prompt_display == 2 then Prompt(recipe[e].pickup_text) end
					if g_KeyPressE == 1 then
						PromptDuration(recipe[e].collected_text,2000)
						PerformLogicConnections(e)
						PlayNon3DSound(e,0)
						SetEntityCollected(e,1,-1)
					end
				end
			end
		end
	end
end
