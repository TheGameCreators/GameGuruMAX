-- Drink From Water v7
-- DESCRIPTION: Attach to an object set to ALWAYS ACTIVE
-- DESCRIPTION: Allows drinking from a water plane within set range.
-- DESCRIPTION: Restores set quantity of health every 2500 milliseconds
-- DESCRIPTION: [PROMPT_TEXT$="Press E to Drink"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [WATER_RANGE=65(1,100)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyHealth"]
-- DESCRIPTION: <Sound0> when drinking.

local drinkwater			= {}
local prompt_text			= {}
local quantity				= {}
local water_range			= {}
local effect 				= {}
local user_global_affected 	= {}
local drinkwater_last		= {}
local drinkwater_cam		= {}
local calchealth			= {}
local drinktime				= {}
local currentvalue 			= {}
local addquantity 			= {}
local status				= {}

function drink_from_water_properties(e, prompt_text, quantity, water_range, effect, user_global_affected)
	drinkwater[e].prompt_text = prompt_text
	drinkwater[e].quantity = quantity
	drinkwater[e].water_range = water_range
	drinkwater[e].effect = effect
	drinkwater[e].user_global_affected = user_global_affected	
end

function drink_from_water_init(e)	
	drinkwater[e] = {}
	drinkwater[e].prompt_text = "Press E to Drink"
	drinkwater[e].quantity = 10
	drinkwater[e].water_range = 80
	drinkwater[e].effect = 1
	drinkwater[e].user_global_affected = "MyHealth"
	drinkwater[e].drinkwater_last = GetTimer(e) + 2500
	drinkwater[e].drinkwater_cam = GetTimer(e)
	currentvalue[e] = 0
	addquantity[e] = 0
	calchealth[e] = drinkwater[e].quantity
	drinktime[e] = 0
	status[e] = "init"
end

function drink_from_water_main(e)
	
	if status[e] == "init" then
		if drinkwater[e].effect == 1 then addquantity[e] = 1 end
		if drinkwater[e].effect == 2 then addquantity[e] = 2 end	
		status[e] = "endinit"
	end	
	
	local PlayerDist = GetPlayerDistance(e)

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
	 
	if math.abs(GetCameraPositionY(0)-GetWaterHeight()) < drinkwater[e].water_range and g_PlayerHealth > 0 and GetTimer(e) > drinkwater[e].drinkwater_last then

		Prompt(drinkwater[e].prompt_text)
				
		if g_KeyPressE == 1 then
			PlaySound(e,0)				
			if drinkwater[e].effect == 1 then calchealth[e] = g_PlayerHealth + drinkwater[e].quantity end
			if drinkwater[e].effect == 2 then calchealth[e] = g_PlayerHealth - drinkwater[e].quantity end
			if calchealth[e] > g_gameloop_StartHealth then
				calchealth[e] = g_gameloop_StartHealth
			end
			SetPlayerHealth(calchealth[e])
			drinkwater[e].drinkwater_last = GetTimer(e) + 2500
			drinkwater[e].drinkwater_cam = GetTimer(e) + 1500
			if addquantity[e] == 1 then
				if drinkwater[e].user_global_affected > "" then 
					if _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] end
					_G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] = currentvalue[e] + drinkwater[e].quantity
					if _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] >= 100 then _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] = 100 end
				end
			end	
			if addquantity[e] == 2 then
				if drinkwater[e].user_global_affected > "" then 
					if _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] end
					_G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] = currentvalue[e] - drinkwater[e].quantity
					if _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..drinkwater[e].user_global_affected.."']"] = 0 end
				end
			end
			drinktime[e] = 0
			currentvalue[e] = 0
		end
	end
end
