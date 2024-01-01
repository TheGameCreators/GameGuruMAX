-- DESCRIPTION: The object will give the player a health boost if collected.
-- Oxygen v3 - thanks to Necrym59
-- DESCRIPTION: The object will give the player a oxygen boost and health boost if collected.
-- DESCRIPTION: Change [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [HEALTH_QUANTITY=10(1,40)]
-- DESCRIPTION: [OXYGEN_QUANTITY=10(1,80)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: Play <Sound0> when the object is picked up by the player.

local oxygen = {}
local prompt_text = {}
local heath_quantity = {}
local oxygen_quantity = {}
local pickup_range = {}
local pickup_style = {}	

function oxygen_properties(e, prompt_text, heath_quantity, oxygen_quantity, pickup_range, pickup_style)
	oxygen[e] = g_Entity[e]
	oxygen[e].prompt_text = prompt_text
	oxygen[e].heath_quantity = heath_quantity
	oxygen[e].oxygen_quantity = oxygen_quantity
	oxygen[e].pickup_range = pickup_range
	oxygen[e].pickup_style = pickup_style	
end

function oxygen_init(e)
	oxygen[e] = g_Entity[e]
	oxygen[e].prompt_text = "E to use"
	oxygen[e].heath_quantity = 10
	oxygen[e].oxygen_quantity = 40
	oxygen[e].pickup_range = 80
	oxygen[e].pickup_style = 1
end

function oxygen_main(e)
	oxygen[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)
	if oxygen[e].pickup_style == 1 then
		if PlayerDist < oxygen[e].pickup_range and g_PlayerHealth > 0 then
			PlaySound(e,0)
			SetPlayerHealth(g_PlayerHealth + oxygen[e].heath_quantity)
			local alreadyhaveair = GetGamePlayerControlDrownTimestamp()-Timer()
			SetGamePlayerControlDrownTimestamp(Timer()+(oxygen[e].oxygen_quantity*100)+alreadyhaveair)
			Destroy(e)		
		end
	end
	if oxygen[e].pickup_style == 2 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < oxygen[e].pickup_range then
			Prompt(oxygen[e].prompt_text)
			if g_KeyPressE == 1 then				
				PlaySound(e,0)
				PerformLogicConnections(e)
				SetPlayerHealth(g_PlayerHealth + oxygen[e].heath_quantity)
				local alreadyhaveair = GetGamePlayerControlDrownTimestamp()-Timer()
				SetGamePlayerControlDrownTimestamp(Timer()+(oxygen[e].oxygen_quantity*100)+alreadyhaveair)
				Destroy(e)
			end
		end
	end
end
