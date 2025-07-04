-- Chest Portable v14  by Necrym59
-- DESCRIPTION: The attached object can be used as a portable chest.
-- DESCRIPTION: Set object as Collectable.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"] [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [USEAGE_TEXT$="Press Y to access Portable Chest"]
-- DESCRIPTION: [USEAGE_KEY$="Y"]
-- DESCRIPTION: When use key is pressed, will open the [@@CHEST_SCREEN$=""(0=hudscreenlist)] eg: HUD Screen 6
-- DESCRIPTION: using [CHEST_CONTAINER$="chestportable"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for pickup

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local lower = string.lower
local chest_portable	= {}
local pickup_text		= {}
local pickup_range		= {}
local pickup_style		= {}
local useage_text		= {}
local useage_key		= {}
local chest_screen 		= {}
local chest_container 	= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local have_chest 		= {}
local selectobj 		= {}
local doonce 			= {}
local tEnt 				= {}
local hl_icon 			= {}
local hl_imgwidth 		= {}
local hl_imgheight 		= {}
local played			= {}
local status 			= {}

function chest_portable_properties(e, pickup_text, pickup_range, pickup_style, useage_text, useage_key, chest_screen, chest_container, prompt_display, item_highlight, highlight_icon_imagefile)
	chest_portable[e] = g_Entity[e]
	chest_portable[e].pickup_text = pickup_text
	chest_portable[e].pickup_range = pickup_range
	chest_portable[e].pickup_style = pickup_style
	chest_portable[e].useage_text = useage_text
	chest_portable[e].useage_key = lower(useage_key)
	chest_portable[e].chest_screen = chest_screen
	chest_portable[e].chest_container = chest_container
	chest_portable[e].prompt_display = prompt_display
	chest_portable[e].item_highlight = item_highlight
	chest_portable[e].highlight_icon = highlight_icon_imagefile	
end

function chest_portable_init(e)
	chest_portable[e] = {}
	chest_portable[e].pickup_text = "E to Pickup"
	chest_portable[e].pickup_range = 80
	chest_portable[e].pickup_style = 2
	chest_portable[e].useage_text = "Press Y to access"
	chest_portable[e].useage_key = "y"
	chest_portable[e].chest_screen = "HUD Screen 7"
	chest_portable[e].chest_container = "chestportable"
	chest_portable[e].prompt_display = 1
	chest_portable[e].item_highlight = 0
	chest_portable[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	have_chest[e] = 0
	selectobj[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	doonce[e] = 0
	played[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	status[e] = "init"	
end

function chest_portable_main(e)

	if status[e] == "init" then
		if chest_portable[e].item_highlight == 3 and chest_portable[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(chest_portable[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(chest_portable[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(chest_portable[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	PlayerDist = GetPlayerDistance(e)

	if chest_portable[e].pickup_style == 1 then
		if have_chest[e] == 0 then
			if GetEntityCollectable(e) == 1 then
				if PlayerDist < chest_portable[e].pickup_range and g_PlayerHealth > 0 and have_chest[e] == 0 then
					have_chest[e] = 1
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end
					SetEntityCollected(e,1)
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
				end
			end
		end
		if have_chest[e] == 1 and doonce[e] == 0 then 
			if chest_portable[e].prompt_display == 1 then PromptLocal(e,chest_portable[e].useage_text) end
			if chest_portable[e].prompt_display == 2 then PromptDuration(chest_portable[e].useage_text,2000) end
			doonce[e] = 1
		end	
	end

	if chest_portable[e].pickup_style == 2 and PlayerDist < chest_portable[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,chest_portable[e].pickup_range,chest_portable[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if have_chest[e] == 0 then
			if GetEntityCollectable(tEnt[e]) == 1 then
				if PlayerDist < chest_portable[e].pickup_range and tEnt[e] == e then
					if chest_portable[e].prompt_display == 1 then PromptLocal(e,chest_portable[e].pickup_text) end
					if chest_portable[e].prompt_display == 2 then Prompt(chest_portable[e].pickup_text) end	
					if g_KeyPressE == 1 then
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
						have_chest[e] = 1						
						SetEntityCollected(tEnt[e],1)
						Hide(e)
						CollisionOff(e)
						if chest_portable[e].prompt_display == 1 then PromptDuration(chest_portable[e].useage_text,2000) end
						if chest_portable[e].prompt_display == 2 then PromptDuration(chest_portable[e].useage_text,2000) end							
					end
				end
			end
		end
	end

	if have_chest[e] > 0 then
		if GetEntityCollected(e) == 1 then
			SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
			if GetCurrentScreen() == -1 then
				-- in the game
				if g_InKey == string.lower(chest_portable[e].useage_key) or g_InKey == tostring(chest_portable[e].useage_key) then
					g_UserGlobalContainer = chest_portable[e].chest_container
					ScreenToggle(chest_portable[e].chest_screen)					
				end
			else
				-- in chest screen
			end
		end
	end
end