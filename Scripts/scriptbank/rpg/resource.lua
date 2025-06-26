-- Resource v9 by Necrym59
-- DESCRIPTION: The object will give the player a crafting resource. Object must be set to 'Collectable' and 'Resource'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Resource collected"]
-- DESCRIPTION: [QUANTITY=1(1,5)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local resource = {}
local pickup_text = {}
local collected_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local status = {}
local tEnt = {}
local selectobj = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function resource_properties(e, pickup_text, collected_text, quantity, pickup_range, pickup_style, prompt_display, item_highlight, highlight_icon_imagefile)
	resource[e].pickup_text = pickup_text
	resource[e].collected_text = collected_text
	resource[e].quantity = quantity
	resource[e].pickup_range = pickup_range
	resource[e].pickup_style = pickup_style
	resource[e].prompt_display = prompt_display
	resource[e].item_highlight = item_highlight
	resource[e].highlight_icon = highlight_icon_imagefile
end

function resource_init(e)
	resource[e] = {}
	resource[e].pickup_text = "E to collect"
	resource[e].collected_text = "Resource collected"
	resource[e].quantity = 1
	resource[e].pickup_range = 80
	resource[e].pickup_style = 1
	resource[e].prompt_display = 1
	resource[e].item_highlight = 0
	resource[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	SetEntityQuantity(e,resource[e].quantity)
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	status[e] = "init"
end

function resource_main(e)

	if status[e] == "init" then
		if resource[e].item_highlight == 3 and resource[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(resource[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(resource[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(resource[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if resource[e].pickup_style == 1 and PlayerDist < resource[e].pickup_range then
		if GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then
				Prompt(resource[e].collected_text)
				PerformLogicConnections(e)
				PlaySound(e,0)
				SetEntityQuantity(e,resource[e].quantity)
				SetEntityCollected(e,1,-1)
			end
		end
	end

	if resource[e].pickup_style == 2 and PlayerDist < resource[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,resource[e].pickup_range,resource[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < resource[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(e) == 2 then
				if GetEntityCollected(e) == 0 then
					if resource[e].prompt_display == 1 then PromptLocal(e,resource[e].pickup_text) end
					if resource[e].prompt_display == 2 then Prompt(resource[e].pickup_text) end
					if g_KeyPressE == 1 then
						Prompt(resource[e].collected_text)
						PerformLogicConnections(e)
						PlaySound(e,0)
						SetEntityQuantity(e,resource[e].quantity)
						SetEntityCollected(e,1,-1)
					end
				end
			end
		end
	end
end
