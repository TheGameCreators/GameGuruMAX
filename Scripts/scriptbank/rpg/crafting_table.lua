-- Crafting Table v15 - Necrym59 and Lee
-- DESCRIPTION: The attached object can be used as a crafting table.
-- DESCRIPTION: When player is within [USE_RANGE=100] distance,
-- DESCRIPTION: show [USE_PROMPT$="Press E to begin crafting"] and
-- DESCRIPTION: when use key is pressed, will open the [@@CRAFT_SCREEN$="HUD Screen 7"(0=hudscreenlist)] Eg: HUD Screen 7
-- DESCRIPTION: using [CRAFT_CONTAINER$="chest"]
-- DESCRIPTION: [CRAFTING_CLASS$=""] The crafting class for this crafting table (eg: "potion") would only allow recipes with (eg: recipe=potion) to be crafted.
-- DESCRIPTION: [PLAYER_LEVEL=0(0,100))] The player level to be able use this crafting table
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\hand.png"]
-- DESCRIPTION: <Sound0> when crafting started.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_CraftingClass = {}

local crafting_table 	= {}
local use_range 		= {}
local use_prompt 		= {}
local craft_screen 		= {}
local craft_container 	= {}
local crafting_class	= {}
local player_level		= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local tEnt 				= {}
local selectobj			= {}
local played			= {}
local cplayerlevel		= {}
local clevelrequired	= {}
local status			= {}
local hl_icon 			= {}
local hl_imgwidth 		= {}
local hl_imgheight 		= {}

function crafting_table_properties(e, use_range, use_prompt, craft_screen, craft_container, crafting_class, player_level, prompt_display, item_highlight, highlight_icon_imagefile)
	crafting_table[e].use_range = use_range
	crafting_table[e].use_prompt = use_prompt
	crafting_table[e].craft_screen = craft_screen
	crafting_table[e].craft_container = craft_container
	crafting_table[e].crafting_class = string.lower(crafting_class)
	crafting_table[e].player_level = player_level or 0 
	crafting_table[e].prompt_display = prompt_display
	crafting_table[e].item_highlight = item_highlight
	crafting_table[e].highlight_icon = highlight_icon_imagefile	
end

function crafting_table_init(e)
	crafting_table[e] = {}
	crafting_table[e].use_range = 100
	crafting_table[e].use_prompt = "Press E to begin crafting"
	crafting_table[e].craft_screen = "HUD Screen 7"
	crafting_table[e].craft_container = "chest"
	crafting_table[e].crafting_class = ""
	crafting_table[e].player_level = 0
	crafting_table[e].prompt_display = 1
	crafting_table[e].item_highlight = 0
	crafting_table[e].highlight_icon = "imagebank\\icons\\hand.png"
	
	tEnt[e] = 0
	g_tEnt = 0 
	selectobj[e] = 0
	played[e] = 0
	status[e] = "init"
	cplayerlevel[e] = 0	
	g_CraftingClass = ""
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function crafting_table_main(e)
	if status[e] == "init" then
		if crafting_table[e].item_highlight == 3 and crafting_table[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(crafting_table[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(crafting_table[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(crafting_table[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		clevelrequired[e] = crafting_table[e].player_level
		status[e] = "endinit"
	end		
	if GetCurrentScreen() == -1 then
		-- in the game		
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < crafting_table[e].use_range then			
			--pinpoint select object--
			module_misclib.pinpoint(e,crafting_table[e].use_range,crafting_table[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < crafting_table[e].use_range and tEnt[e] == e then
			if _G["g_UserGlobal['".."MyPlayerLevel".."']"] ~= nil then cplayerlevel[e] = _G["g_UserGlobal['".."MyPlayerLevel".."']"] end
			if cplayerlevel[e] < clevelrequired[e] then
				if crafting_table[e].prompt_display == 1 then PromptLocal(e,"You need to be level "..clevelrequired[e].." to use") end
				if crafting_table[e].prompt_display == 2 then Prompt("You need to be level "..clevelrequired[e].." to use") end
			end	
			if cplayerlevel[e] >= clevelrequired[e] then
				if crafting_table[e].prompt_display == 1 then PromptLocal(e, crafting_table[e].use_prompt) end
				if crafting_table[e].prompt_display == 2 then Prompt(crafting_table[e].use_prompt) end		
				if g_KeyPressE == 1 then
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end
					g_UserGlobalContainer = crafting_table[e].craft_container
					g_CraftingClass = crafting_table[e].crafting_class
					ScreenToggle(crafting_table[e].craft_screen)				
				end
			end
		end		
	else
		-- in craft screen
	end	
end
