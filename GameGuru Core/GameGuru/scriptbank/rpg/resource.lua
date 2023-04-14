-- Resource v3
-- DESCRIPTION: The object will give the player a crafting resource. Object must be set to 'Collectable' and 'Resource'.
-- DESCRIPTION: [PICKUP_TEXT$="E to harvest"]
-- DESCRIPTION: [COLLECTED_TEXT$="Resource harvested"]
-- DESCRIPTION: [QUANTITY=1(1,5)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: <Sound0> for harvesting sound.

local g_resource = {}
local pickup_text = {}
local collected_text = {}
local quantity = {}
local pickup_range = {}

function resource_properties(e, pickup_text, collected_text, quantity, pickup_range)
	g_resource[e].pickup_text = pickup_text
	g_resource[e].collected_text = collected_text
	g_resource[e].quantity = quantity
	g_resource[e].pickup_range = pickup_range
end

function resource_init(e)
	g_resource[e] = {}
	resource_properties(e, "E to harvest", "Resource harvested", 1, 80)
	SetEntityQuantity(e,g_resource[e].quantity)
end

function resource_main(e)
	PlayerDist = GetPlayerDistance(e)	
	local LookingAt = GetPlrLookingAtExThreshold(e,1,10)
	if LookingAt == 1 and PlayerDist < g_resource[e].pickup_range then
		if GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then
				Prompt(g_resource[e].pickup_text)
				if g_KeyPressE == 1 then
					Prompt(g_resource[e].collected_text)
					PerformLogicConnections(e)
					PlaySound(e,0)
					SetEntityCollected(e,1,-1)
				end
			end
		end
	end	
end
