-- Crafting Table v3
-- DESCRIPTION: The attached object can be used as a crafting table.
-- DESCRIPTION: When player is within [USE_RANGE=100] distance,
-- DESCRIPTION: show [USE_PROMPT$="Press E to begin crafting"] and
-- DESCRIPTION: when use key is pressed, will open the [CRAFT_SCREEN$="HUD Screen 7"]
-- DESCRIPTION: using [CRAFT_CONTAINER$="chest"]
-- DESCRIPTION: <Sound0> when crafting started.

local crafting_table 	= {}
local use_range 		= {}
local use_prompt 		= {}
local craft_screen 		= {}
local craft_container 	= {}

function crafting_table_properties(e, use_range, use_prompt, craft_screen, craft_container)
	crafting_table[e].use_range = use_range
	crafting_table[e].use_prompt = use_prompt
	crafting_table[e].craft_screen = craft_screen
	crafting_table[e].craft_container = craft_container
end

function crafting_table_init(e)
	crafting_table[e] = {}
	crafting_table[e].use_range = 100
	crafting_table[e].use_prompt = "Press E to begin crafting"
	crafting_table[e].craft_screen = "HUD Screen 7"
	crafting_table[e].craft_container = "chest"
end

function crafting_table_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < crafting_table[e].use_range then
			PromptDuration(crafting_table[e].use_prompt ,1000)	
			if g_KeyPressE == 1 then
				g_UserGlobalContainer = crafting_table[e].craft_container
				ScreenToggle(crafting_table[e].craft_screen)
			end
		end
	else
		-- in craft screen
	end	
end
