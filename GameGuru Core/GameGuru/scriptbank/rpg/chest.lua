-- Chest v1
-- DESCRIPTION: When player is within [RANGE=100] distance, show [CHEST_PROMPT$="Press E to open"] and when E is pressed, player will open [CHEST_SCREEN$="HUD Screen 6"] using [CHEST_CONTAINER$="chest"].
-- DESCRIPTION: <Sound0> for opening sound.

local g_chest = {}

function chest_init(e)
	g_chest[e] = {}
	chest_properties(e,100,"Press E to open","HUD Screen 6","chest")
end

function chest_properties(e, range, chestprompt, chestscreen, chestcontainer)
	g_chest[e]['range'] = range
	g_chest[e]['chestprompt'] = chestprompt
	g_chest[e]['chestscreen'] = chestscreen
	g_chest[e]['chestcontainer'] = chestcontainer
end

function chest_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < g_chest[e]['range'] then
			PromptDuration(g_chest[e]['chestprompt'] ,1000)	
			if g_KeyPressE == 1 then
				g_UserGlobalContainer = g_chest[e]['chestcontainer']
				ScreenToggle(g_chest[e]['chestscreen'])
			end
		end
	else
		-- in chest screen
	end
end
