-- Crafting Table v1
-- DESCRIPTION: When player is within [RANGE=100] distance, show [CRAFT_PROMPT$="Press E to start crafting"] and when E is pressed, player will open [CRAFT_SCREEN$="HUD Screen 7"] using [CRAFT_CONTAINER$="chest"].
-- DESCRIPTION: <Sound0> when crafting started.

local g_crafting_table = {}

function crafting_table_init(e)
	g_crafting_table[e] = {}
	crafting_table_properties(e,100,"Press E to start crafting","HUD Screen 7","chest")
end

function crafting_table_properties(e, range, craftprompt, craftscreen, craftcontainer)
	g_crafting_table[e]['range'] = range
	g_crafting_table[e]['craftprompt'] = craftprompt
	g_crafting_table[e]['craftscreen'] = craftscreen
	g_crafting_table[e]['craftcontainer'] = craftcontainer
end

function crafting_table_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < g_crafting_table[e]['range'] then
			PromptDuration(g_crafting_table[e]['craftprompt'] ,1000)	
			if g_KeyPressE == 1 then
				g_UserGlobalContainer = g_crafting_table[e]['craftcontainer']
				ScreenToggle(g_crafting_table[e]['craftscreen'])
			end
		end
	else
		-- in craft screen
	end
end
