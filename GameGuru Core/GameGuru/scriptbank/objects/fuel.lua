-- Fuel v2
-- DESCRIPTION: The attached object will give the player a fuel resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="Collected Fuel"]
-- DESCRIPTION: [AMOUNT=10(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=50(1,100)]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

g_fuel = {}
local fuel = {}
local prompt_text = {}
local amount = {}
local pickup_range = {}	
local total = {}		

function fuel_properties(e, prompt_text, amount, pickup_range)
	fuel[e] = g_Entity[e]
	fuel[e].prompt_text = prompt_text
	fuel[e].amount = amount
	fuel[e].pickup_range = pickup_range
end

function fuel_init_name(e)	
	fuel[e] = g_Entity[e]	
	fuel[e].prompt_text = "Collected Fuel"
	fuel[e].prompt_text = prompt_text
	fuel[e].amount = amount
	fuel[e].pickup_range = pickup_range	
	g_fuel = 0
end

function fuel_main(e)
	fuel[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if PlayerDist < fuel[e].pickup_range and g_PlayerHealth > 0 then
		Prompt(fuel[e].prompt_text)
		PlaySound(e,0)
		PerformLogicConnections(e)		
		g_fuel = g_fuel + fuel[e].amount		
		Destroy(e)
	end	
end

