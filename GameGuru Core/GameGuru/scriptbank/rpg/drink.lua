-- Drink v2
-- DESCRIPTION: The object will give the player a health (QUANTITY) if an object (e.g a fountain) is drunk from, every 2500 milliseconds)
-- DESCRIPTION: Change [PROMPT_TEXT$="Press E to Drink"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [DRINK_RANGE=80(1,100)]
-- DESCRIPTION: <Sound0> when drinking.

local drink = {}
local prompt_text = {}
local quantity = {}
local drink_range = {}	
local drink_last = {}
local drink_cam = {}
local calchealth = {}
local drinktime = {}

function drink_properties(e, prompt_text, quantity, drink_range)
	drink[e] = g_Entity[e]
	drink[e].prompt_text = prompt_text
	drink[e].quantity = quantity
	drink[e].drink_range = drink_range		
end

function drink_init(e)
	drink[e] = g_Entity[e]
	drink[e].prompt_text = "Press E to Drink"
	drink[e].quantity = 10
	drink[e].drink_range = 80
	drink[e].drink_last = GetTimer(e) + 2500
	drink[e].drink_cam = GetTimer(e)
	calchealth = drink[e].quantity
	drinktime[e] = 0
end

function drink_main(e)
	drink[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)

	if GetTimer(e) < drink[e].drink_cam then		
		SetCameraPosition(0,GetCameraPositionX(0),GetCameraPositionY(0)-drinktime[e],GetCameraPositionZ(0))
		drinktime[e] = drinktime[e] + 0.5
		if drinktime[e] >= 20 then  drinktime[e] = 20 end			
	end
	if GetTimer(e) > drink[e].drink_cam then		
		SetCameraPosition(0,GetCameraPositionX(0),GetCameraPositionY(0)-drinktime[e],GetCameraPositionZ(0))
		drinktime[e] = drinktime[e] - 0.5
		if drinktime[e] < 0 then  drinktime[e] = 0 end			
	end
		
	if PlayerDist < drink[e].drink_range and g_PlayerHealth > 0 and GetTimer(e) > drink[e].drink_last  then

		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 then
			Prompt(drink[e].prompt_text)
				
			if g_KeyPressE == 1 then
				PlaySound(e,0)
				calchealth = g_PlayerHealth + drink[e].quantity
				if calchealth > g_gameloop_StartHealth then
					calchealth = g_gameloop_StartHealth
				end
				SetPlayerHealth(calchealth)
				drink[e].drink_last = GetTimer(e) + 2500
				drink[e].drink_cam = GetTimer(e) + 1500				
			end
			drinktime[e] = 0
		end
	end
end
