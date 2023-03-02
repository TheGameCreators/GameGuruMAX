-- Drink From Water v2
-- DESCRIPTION: Attach to ALWAYS ACTIVE object. Allows drinking from a water plane and within DRINKWATER_RANGE. Restores QUANTITY Health every 2500 milliseconds
-- DESCRIPTION: Change [PROMPT_TEXT$="Press E to Drink"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [DRINKWATER_RANGE=65(1,100)]

local drinkwater = {}
local prompt_text = {}
local quantity = {}
local drinkwater_last = {}
local drinkwater_cam = {}
local calchealth = {}
local drinktime = {}

function drink_from_water_properties(e, prompt_text, quantity, drinkwater_range)
	drinkwater[e] = g_Entity[e]
	drinkwater[e].prompt_text = prompt_text
	drinkwater[e].quantity = quantity
	drinkwater[e].drinkwater_range = drinkwater_range		
end

function drink_from_water_init(e)	
	drinkwater[e] = g_Entity[e]
	drinkwater[e].prompt_text = "Press E to Drink"
	drinkwater[e].quantity = 10
	drinkwater[e].drinkwater_range = 80
	drinkwater[e].drinkwater_last = GetTimer(e) + 2500
	drinkwater[e].drinkwater_cam = GetTimer(e)
	calchealth = drinkwater[e].quantity
	drinktime[e] = 0
end

function drink_from_water_main(e)
	drinkwater[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)

	if GetTimer(e) < drinkwater[e].drinkwater_cam then		
		SetCameraPosition(0,GetCameraPositionX(0),GetCameraPositionY(0)-drinktime[e],GetCameraPositionZ(0))
		drinktime[e] = drinktime[e] + 0.5
		if drinktime[e] >= 20 then  drinktime[e] = 20 end			
	end
	if GetTimer(e) > drinkwater[e].drinkwater_cam then		
		SetCameraPosition(0,GetCameraPositionX(0),GetCameraPositionY(0)-drinktime[e],GetCameraPositionZ(0))
		drinktime[e] = drinktime[e] - 0.5
		if drinktime[e] < 0 then  drinktime[e] = 0 end			
	end
	 
	if math.abs(GetCameraPositionY(0)-GetWaterHeight()) < drinkwater[e].drinkwater_range and g_PlayerHealth > 0 and GetTimer(e) > drinkwater[e].drinkwater_last then

		Prompt(drinkwater[e].prompt_text)
				
		if g_KeyPressE == 1 then
			PlaySound(e,0)
				
			calchealth = g_PlayerHealth + drinkwater[e].quantity

			if calchealth > g_gameloop_StartHealth then
				calchealth = g_gameloop_StartHealth
			end
			SetPlayerHealth(calchealth)
			drinkwater[e].drinkwater_last = GetTimer(e) + 2500
			drinkwater[e].drinkwater_cam = GetTimer(e) + 1500
		end
	end
end
