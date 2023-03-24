-- Recipe v1
-- DESCRIPTION: The object will give the player a recipe to collect. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Recipe collected"]
-- DESCRIPTION: [QUANTITY=1(1,5)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: <Sound0> for collected sound.

local g_recipe = {}

function recipe_properties(e, pickup_text, collected_text, quantity, pickup_range)
	g_recipe[e].pickup_text = pickup_text
	g_recipe[e].collected_text = collected_text
	g_recipe[e].quantity = quantity
	g_recipe[e].pickup_range = pickup_range
end

function recipe_init(e)
	g_recipe[e] = {}
	recipe_properties(e, "E to collect", "Recipe collected", 1, 80)
end

function recipe_main(e)
	PlayerDist = GetPlayerDistance(e)	
	local LookingAt = GetPlrLookingAtExThreshold(e,1,10)
	if LookingAt == 1 and PlayerDist < g_recipe[e].pickup_range then
		if GetEntityCollectable(e) == 1 then
			if GetEntityCollected(e) == 0 then
				Prompt(g_recipe[e].pickup_text)
				if g_KeyPressE == 1 then
					Prompt(g_recipe[e].collected_text)
					PerformLogicConnections(e)
					PlaySound(e,0)
					SetEntityCollected(e,1,-1)
				end
			end
		end
	end	
end
