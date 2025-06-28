-- Collect Object v11
-- DESCRIPTION: Will allow collection of an object. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [COLLECTED_TEXT$="Item collected"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_ResnodeCollected = {}

local collect_object 	= {}
local pickup_text 		= {}
local pickup_range 		= {}
local pickup_style 		= {}
local collected_text 	= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local tEnt = {}
local selectobj = {}
local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function collect_object_properties(e, pickup_text, pickup_range, pickup_style, collected_text, prompt_display, item_highlight, highlight_icon_imagefile)
	collect_object[e].pickup_text = pickup_text
	collect_object[e].pickup_range = pickup_range
	collect_object[e].pickup_style = pickup_style
	collect_object[e].collected_text = collected_text
	collect_object[e].prompt_display = prompt_display
	collect_object[e].item_highlight = item_highlight
	collect_object[e].highlight_icon = highlight_icon_imagefile
end

function collect_object_init(e)
	collect_object[e] = {}
	collect_object[e].pickup_text = "E to collect"
	collect_object[e].pickup_range = 80
	collect_object[e].pickup_style = 2
	collect_object[e].collected_text = "Item collected"
	collect_object[e].prompt_display = 1
	collect_object[e].item_highlight = 0
	collect_object[e].highlight_icon = "imagebank\\icons\\pickup.png"

	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function collect_object_main(e)

	if status[e] == "init" then
		if collect_object[e].item_highlight == 3 and collect_object[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(collect_object[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(collect_object[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(collect_object[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if collect_object[e].pickup_style == 1 and PlayerDist < collect_object[e].pickup_range then
		if GetEntityCollectable(e) == 1 or GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then
				if collect_object[e].prompt_display == 1 then PromptLocal(e,collect_object[e].collected_text) end
				if collect_object[e].prompt_display == 2 then Prompt(collect_object[e].collected_text) end
				PlaySound(e,0)
				PerformLogicConnections(e)
				SetEntityCollected(e,1)
				g_ResnodeCollected = e
			end
		end
	end

	if collect_object[e].pickup_style == 2 and PlayerDist < collect_object[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,collect_object[e].pickup_range,collect_object[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < collect_object[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if GetEntityCollected(tEnt[e]) == 0 then
					if collect_object[e].prompt_display == 1 then PromptLocal(e,collect_object[e].pickup_text) end
					if collect_object[e].prompt_display == 2 then Prompt(collect_object[e].pickup_text) end
					if g_KeyPressE == 1 then
						PlaySound(e,0)
						if collect_object[e].prompt_display == 1 then PromptLocal(e,collect_object[e].collected_text) end
						if collect_object[e].prompt_display == 2 then Prompt(collect_object[e].collected_text) end
						PerformLogicConnections(e)
						SetEntityCollected(tEnt[e],1)
						g_ResnodeCollected = e
					end
				end
			end
		end
	end
end
