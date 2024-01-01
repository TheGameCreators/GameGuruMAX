-- Drink v5
-- DESCRIPTION: The object will give the player a health boost or reduction if an object (e.g a fountain) is drunk from, every 2500 milliseconds)
-- DESCRIPTION: and can also effect a user global if required.
-- DESCRIPTION: Change [PROMPT_TEXT$="Press E to Drink"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [DRINK_RANGE=80(1,100)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyHealth"]
-- DESCRIPTION: <Sound0> when drinking.

local drink 				= {}
local prompt_text 			= {}
local quantity 				= {}
local drink_range 			= {}
local effect 				= {}
local user_global_affected 	= {}	

local currentvalue 			= {}
local addquantity 			= {}
local drink_last 			= {}
local drink_cam 			= {}
local calchealth	 		= {}
local drinktime 			= {}
local status 				= {}

function drink_properties(e, prompt_text, quantity, drink_range, effect, user_global_affected)
	drink[e] = g_Entity[e]
	drink[e].prompt_text = prompt_text
	drink[e].quantity = quantity
	drink[e].drink_range = drink_range
	drink[e].effect = effect
	drink[e].user_global_affected = user_global_affected
end

function drink_init(e)
	drink[e] = g_Entity[e]
	drink[e].prompt_text = "Press E to Drink"
	drink[e].quantity = 10
	drink[e].drink_range = 80
	drink[e].effect = 1
	drink[e].user_global_affected = "MyHealth"
	
	
	drink[e].drink_last = GetTimer(e) + 2500
	drink[e].drink_cam = GetTimer(e)
	drinktime[e] = 0
	drinktime[e] = 0
	currentvalue[e] = 0
	addquantity[e] = 0
	calchealth[e] = 0
	status[e] = "init"
end

function drink_main(e)
	drink[e] = g_Entity[e]
	
	if status[e] == "init" then
		if drink[e].effect == 1 then addquantity[e] = 1 end
		if drink[e].effect == 2 then addquantity[e] = 2 end	
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)

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
				if drink[e].effect == 1 then calchealth[e] = g_PlayerHealth + drink[e].quantity end
				if drink[e].effect == 2 then calchealth[e] = g_PlayerHealth - drink[e].quantity end
				if calchealth[e] > g_gameloop_StartHealth then
					calchealth[e] = g_gameloop_StartHealth
				end
				SetPlayerHealth(calchealth[e])
				drink[e].drink_last = GetTimer(e) + 2500
				drink[e].drink_cam = GetTimer(e) + 1500				
						
				if addquantity[e] == 1 then
					if drink[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..drink[e].user_global_affected.."']"] = currentvalue[e] + drink[e].quantity
						if _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] >= 100 then _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] = 100 end
					end
				end	
				if addquantity[e] == 2 then
					if drink[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..drink[e].user_global_affected.."']"] = currentvalue[e] - drink[e].quantity
						if _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..drink[e].user_global_affected.."']"] = 0 end
					end
				end
			end
			drinktime[e] = 0
			currentvalue[e] = 0
		end
	end
end