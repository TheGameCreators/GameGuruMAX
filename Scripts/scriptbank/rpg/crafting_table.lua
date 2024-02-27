-- Crafting Table v10
-- DESCRIPTION: The attached object can be used as a crafting table.
-- DESCRIPTION: When player is within [USE_RANGE=100] distance,
-- DESCRIPTION: show [USE_PROMPT$="Press E to begin crafting"] and
-- DESCRIPTION: when use key is pressed, will open the [CRAFT_SCREEN$="HUD Screen 7"]
-- DESCRIPTION: using [CRAFT_CONTAINER$="chest"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> when crafting started.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local crafting_table 	= {}
local use_range 		= {}
local use_prompt 		= {}
local craft_screen 		= {}
local craft_container 	= {}
local tEnt 				= {}
local selectobj			= {}
local played			= {}

function crafting_table_properties(e, use_range, use_prompt, craft_screen, craft_container, prompt_display)
	crafting_table[e].use_range = use_range
	crafting_table[e].use_prompt = use_prompt
	crafting_table[e].craft_screen = craft_screen
	crafting_table[e].craft_container = craft_container
	crafting_table[e].prompt_display = prompt_display
end

function crafting_table_init(e)
	crafting_table[e] = {}
	crafting_table[e].use_range = 100
	crafting_table[e].use_prompt = "Press E to begin crafting"
	crafting_table[e].craft_screen = "HUD Screen 7"
	crafting_table[e].craft_container = "chest"
	crafting_table[e].prompt_display = 1
	tEnt[e] = 0
	g_tEnt = 0 
	selectobj[e] = 0
	played[e] = 0
end

function crafting_table_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < crafting_table[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,crafting_table[e].use_range,300)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < crafting_table[e].use_range and tEnt[e] ~= 0 then
			if crafting_table[e].prompt_display == 1 then PromptLocal(e, crafting_table[e].use_prompt) end
			if crafting_table[e].prompt_display == 2 then Prompt(crafting_table[e].use_prompt) end		
			if g_KeyPressE == 1 then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_UserGlobalContainer = crafting_table[e].craft_container
				ScreenToggle(crafting_table[e].craft_screen)
			end
		end
	else
		-- in craft screen
	end	
end
