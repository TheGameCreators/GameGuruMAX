-- DESCRIPTION: The object will give the player a health boost if collected.
-- v3
-- DESCRIPTION: Change [PROMPT_TEXT$="Health Collected"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: <Sound0> for collection

local health = {}
local prompt_text = {}
local quantity = {}
local pickup_range = {}	

function health_properties(e, prompt_text, quantity, pickup_range, pickup_style)
	health[e] = g_Entity[e]
	health[e].prompt_text = prompt_text
	health[e].quantity = quantity
	health[e].pickup_range = pickup_range
	health[e].pickup_style = pickup_style
end

function health_init(e)
	health[e] = g_Entity[e]
	health[e].prompt_text = ""
	health[e].quantity = 10
	health[e].pickup_range = 80
	health[e].pickup_style = 1
end

function health_main(e)
	health[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if health[e].pickup_style == 1 then
		if PlayerDist < health[e].pickup_range and g_PlayerHealth > 0 then			
			PlaySound(e,0)
			PerformLogicConnections(e)
			Prompt(health[e].prompt_text)
			SetPlayerHealth(g_PlayerHealth + health[e].quantity)	
			Destroy(e)
		end
	end
	if health[e].pickup_style == 2 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < health[e].pickup_range then
			Prompt(health[e].prompt_text)
			if g_KeyPressE == 1 then				
				PlaySound(e,0)
				PerformLogicConnections(e)
				SetPlayerHealth(g_PlayerHealth + health[e].quantity)	
				Destroy(e)
			end
		end
	end
end
