-- Crafting Kit v12   by Necrym59
-- DESCRIPTION: The attached object can be used as a portable crafting kit.
-- DESCRIPTION: Set object as Collectable.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"] [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual, 3=Already Carrying)]
-- DESCRIPTION: [USEAGE_TEXT$="Press U to begin crafting"]
-- DESCRIPTION: [USEAGE_KEY$="U"]
-- DESCRIPTION: When use key is pressed, will open the [@@CRAFT_SCREEN$="HUD Screen 7"(0=hudscreenlist)] Eg: HUD Screen 7
-- DESCRIPTION: using [CRAFT_CONTAINER$="chest"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> for pickup

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local lower = string.lower
g_craftstation = {}
local crafting_kit		= {}
local pickup_text		= {}
local pickup_range		= {}
local pickup_style		= {}
local useage_text		= {}
local craft_screen 		= {}
local craft_container 	= {}
local prompt_display 	= {}
local item_highlight 	= {}

local have_crafting_kit = {}
local status 			= {}
local selectobj 		= {}
local doonce 			= {}
local tEnt 				= {}
local played			= {}

function crafting_kit_properties(e, pickup_text, pickup_range, pickup_style, useage_text, useage_key, craft_screen, craft_container, prompt_display, item_highlight)
	crafting_kit[e].pickup_text = pickup_text
	crafting_kit[e].pickup_range = pickup_range
	crafting_kit[e].pickup_style = pickup_style
	crafting_kit[e].useage_text = useage_text
	crafting_kit[e].useage_key = lower(useage_key)
	crafting_kit[e].craft_screen = craft_screen
	crafting_kit[e].craft_container = craft_container
	crafting_kit[e].prompt_display = prompt_display
	crafting_kit[e].item_highlight = item_highlight
end

function crafting_kit_init(e)
	crafting_kit[e] = {}
	crafting_kit[e].pickup_text = "E to Pickup"
	crafting_kit[e].pickup_range = 80
	crafting_kit[e].pickup_style = 2
	crafting_kit[e].useage_text = "Press U to begin crafting"
	crafting_kit[e].useage_key = "U"
	crafting_kit[e].craft_screen = "HUD Screen 7"
	crafting_kit[e].craft_container = "chest"
	crafting_kit[e].prompt_display = 1
	crafting_kit[e].item_highlight = 0
	
	have_crafting_kit[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	doonce[e] = 0
	played[e] = 0
	status[e] = "init"
end

function crafting_kit_main(e)

	if status[e] == "init" then
		if crafting_kit[e].pickup_style == 3 then
			have_crafting_kit[e] = 1
			SetEntityCollected(e,1)
			SetEntityAlwaysActive(e,1)
			CollisionOff(e)
			Hide(e)
		end
		status[e] = "endinit"
	end

	PlayerDist = GetPlayerDistance(e)

	if crafting_kit[e].pickup_style == 1 then
		if have_crafting_kit[e] == 0 then
			if GetEntityCollectable(e) == 1 then
				if PlayerDist < crafting_kit[e].pickup_range and g_PlayerHealth > 0 and have_crafting_kit[e] == 0 then
					have_crafting_kit[e] = 1
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end
					SetEntityCollected(e,1)
					CollisionOff(e)
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
					Hide(e)
				end
			end
		end
	end

	if crafting_kit[e].pickup_style == 2 and PlayerDist < crafting_kit[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,crafting_kit[e].pickup_range,crafting_kit[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if have_crafting_kit[e] == 0 then
			if GetEntityCollectable(tEnt[e]) == 1 then
				if PlayerDist < crafting_kit[e].pickup_range and tEnt[e] ~= 0 then
					if crafting_kit[e].prompt_display == 1 then PromptLocal(e,crafting_kit[e].pickup_text) end
					if crafting_kit[e].prompt_display == 2 then Prompt(crafting_kit[e].pickup_text) end				
					if g_KeyPressE == 1 then
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
						have_crafting_kit[e] = 1						
						SetEntityCollected(tEnt[e],1)
					end
				end
			end
		end
	end
	
	if have_crafting_kit[e] == 1 and doonce[e] == 0 then
		PromptDuration(crafting_kit[e].useage_text,2000)
		doonce[e] = 1
	end	

	if have_crafting_kit[e] > 0 then
		if GetEntityCollected(e) == 1 then
			SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
			if GetCurrentScreen() == -1 then
				-- in the game
				if GetInKey() == string.upper(crafting_kit[e].useage_key) or GetInKey() == string.lower(crafting_kit[e].useage_key) then
					g_craftstation = 1	-- for use of limiting crafts (not yet activated)
					g_UserGlobalContainer = crafting_kit[e].craft_container
					ScreenToggle(crafting_kit[e].craft_screen)					
				end
			else
				-- in craft screen
			end
		end
	end
	if have_crafting_kit[e] == 0 then g_craftstation = 0 end
end