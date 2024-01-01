-- Bottled_Item v6  by Necrym59
-- DESCRIPTION: The object will give the player a health boost or loss if consumed.
-- DESCRIPTION: and can also effect a user global if required.
-- DESCRIPTION: [PROMPT_TEXT$="Press E to consume"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyHealth"]
-- DESCRIPTION: [POISONING_EFFECT!=0]
-- DESCRIPTION: <Sound0> when consuming.
-- DESCRIPTION: <Sound1> when poisoned.
local U = require "scriptbank\\utillib"

local bottle 				= {}
local prompt_text 			= {}
local quantity 				= {}
local pickup_range 			= {}
local effect 				= {}
local user_global_affected 	= {}	

local tEnt 					= {}
local selectobj 			= {}
local currentvalue 			= {}
local addquantity 			= {}
local calchealth	 		= {}
local poisoned				= {}
local doonce				= {}
local actioned				= {}
local status 				= {}

function bottled_item_properties(e, prompt_text, quantity, pickup_range, effect, user_global_affected, poisoning_effect)
	bottle[e] = g_Entity[e]
	bottle[e].prompt_text = prompt_text
	bottle[e].quantity = quantity
	bottle[e].pickup_range = pickup_range
	bottle[e].effect = effect
	bottle[e].user_global_affected = user_global_affected
	bottle[e].poisoned = poisoning_effect
end

function bottled_item_init(e)
	bottle[e] = {}
	bottle[e].prompt_text = "Press E to consume"
	bottle[e].quantity = 10
	bottle[e].pickup_range = 80
	bottle[e].effect = 1
	bottle[e].user_global_affected = "MyHealth"
	bottle[e].poisoned = 0
	
	currentvalue[e] = 0
	addquantity[e] = 0
	calchealth[e] = 0
	poisoned[e] = 0	
	doonce[e] = 0
	actioned[e] = 0	
	status[e] = "init"
end

function bottled_item_main(e)
	bottle[e] = g_Entity[e]
	
	if status[e] == "init" then
		if bottle[e].effect == 1 then addquantity[e] = 1 end
		if bottle[e].effect == 2 then addquantity[e] = 2 end
		poisoned[e] = bottle[e].quantity * 30
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	
	if PlayerDist < bottle[e].pickup_range and GetEntityVisibility(e) == 1 then
		--pinpoint select object--
		
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,bottle[e].pickup_range
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
				tEnt[e] = e
			end
		end
		if selectobj[e] == 0 or selectobj[e] == nil then
			tEnt[e] = 0
			TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
		end
		--end pinpoint select object--
	
		if PlayerDist < bottle[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if doonce[e] == 0 then Prompt(bottle[e].prompt_text) end	
			if g_KeyPressE == 1 then
				doonce[e] = 1
				if actioned[e] == 0 then
					PlaySound(e,0)
					if bottle[e].effect == 1 then calchealth[e] = g_PlayerHealth + bottle[e].quantity end
					if bottle[e].effect == 2 then calchealth[e] = g_PlayerHealth - bottle[e].quantity end
					if calchealth[e] > g_gameloop_StartHealth then
						calchealth[e] = g_gameloop_StartHealth
					end
					SetPlayerHealth(calchealth[e])
					actioned[e] = 1
				end					
				if addquantity[e] == 1 then
					if bottle[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..bottle[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..bottle[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..bottle[e].user_global_affected.."']"] = currentvalue[e] + bottle[e].quantity
					end
					Hide(e)
					CollisionOff(e)
					Destroy(e)
				end	
				if addquantity[e] == 2 then
					if bottle[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..bottle[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..bottle[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..bottle[e].user_global_affected.."']"] = currentvalue[e] - bottle[e].quantity
						if bottle[e].poisoned == 1 then
							Hide(e)
							CollisionOff(e)
							status[e] = "poisoned"
						end
						if bottle[e].poisoned == 0 then
							Hide(e)
							CollisionOff(e)
							Destroy(e)
						end
					end	
				end							
			end
		end		
	end
	if status[e] == "poisoned" then
		PlaySound(e,1)
		if poisoned[e] > 0 then				
			GamePlayerControlSetShakeTrauma(565.0)
			GamePlayerControlSetShakePeriod(960.00)		
			poisoned[e] = poisoned[e]-1			
		end
		if poisoned[e] == 0 then
			GamePlayerControlSetShakeTrauma(0.0)
			GamePlayerControlSetShakePeriod(0.00)
			StopSound(e,0)
			StopSound(e,1)							
			Destroy(e)
		end
	end
end