-- Eat v9 by Necrym59
-- DESCRIPTION: The object will give the player a health boost or loss if consumed and can also effect a user global if required.
-- DESCRIPTION: Set AlwaysActive = On.
-- DESCRIPTION: [PROMPT_TEXT$="Press E to consume"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyGlobal"]
-- DESCRIPTION: [POISONING_EFFECT!=0]
-- DESCRIPTION: [RESPAWNING=0!=0]
-- DESCRIPTION: [RESPAWN_TIME=1(1,60)] in minutes
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> when consuming.
-- DESCRIPTION: <Sound1> when poisoned.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local eat 					= {}
local prompt_text 			= {}
local quantity 				= {}
local eat_pickup_range 		= {}
local effect 				= {}
local user_global_affected 	= {}
local poisoning_effect 		= {}
local respawning 			= {}
local respawn_time 			= {}
local prompt_display 		= {}

local tEnt 					= {}
local selectobj 			= {}
local currentvalue 			= {}
local addquantity 			= {}
local calchealth	 		= {}
local poisoned				= {}
local doonce				= {}
local pressed				= {}
local actioned				= {}
local respawntime			= {}
local status 				= {}

function eat_properties(e, prompt_text, quantity, pickup_range, effect, user_global_affected, poisoning_effect, respawning, respawn_time, prompt_display)
	eat[e].prompt_text = prompt_text
	eat[e].quantity = quantity
	eat[e].pickup_range = pickup_range
	eat[e].effect = effect
	eat[e].user_global_affected = user_global_affected
	eat[e].poisoned = poisoning_effect
	eat[e].respawning = respawning	
	eat[e].respawn_time = respawn_time
	eat[e].prompt_display = prompt_display
end

function eat_init(e)
	eat[e] = {}
	eat[e].prompt_text = "Press E to eat"
	eat[e].quantity = 10
	eat[e].pickup_range = 80
	eat[e].effect = 1
	eat[e].user_global_affected = "MyGlobal"
	eat[e].poisoned = 0
	eat[e].respawning = 0	
	eat[e].respawn_time = 10
	eat[e].prompt_display = 1	
	
	currentvalue[e] = 0
	addquantity[e] = 0
	calchealth[e] = 0
	poisoned[e] = 0
	doonce[e] = 0
	pressed[e] = 0
	actioned[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	respawntime[e] = 0
	status[e] = "init"
end

function eat_main(e)
	
	if status[e] == "init" then
		if eat[e].effect == 1 then addquantity[e] = 1 end
		if eat[e].effect == 2 then addquantity[e] = 2 end
		poisoned[e] = eat[e].quantity * 30
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)	
	
	if PlayerDist < eat[e].pickup_range and GetEntityVisibility(e) == 1 then
		--pinpoint select object--
		module_misclib.pinpoint(e,eat[e].pickup_range,300)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		
		if PlayerDist < eat[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if doonce[e] == 0 then
				if eat[e].prompt_display == 1 then PromptLocal(e,eat[e].prompt_text) end
				if eat[e].prompt_display == 2 then Prompt(eat[e].prompt_text) end
			end	
			if g_KeyPressE == 1 and pressed[e] == 0 then
				doonce[e] = 1
				pressed[e] = 1
				if actioned[e] == 0 then
					PlaySound(e,0)
					if eat[e].effect == 1 then calchealth[e] = g_PlayerHealth + eat[e].quantity end
					if eat[e].effect == 2 then calchealth[e] = g_PlayerHealth - eat[e].quantity end
					if calchealth[e] > g_gameloop_StartHealth then
						calchealth[e] = g_gameloop_StartHealth
					end
					SetPlayerHealth(calchealth[e])
					actioned[e] = 1
				end
				if addquantity[e] == 1 then
					if eat[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..eat[e].user_global_affected.."']"] = currentvalue[e] + eat[e].quantity
						if _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] >= 100 then _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] = 100 end													
					end
					CollisionOff(e)			
					Hide(e)
					if eat[e].respawning == 1 then respawntime[e] = g_Time + ((eat[e].respawn_time*1000)*60) end
				end	
				if addquantity[e] == 2 then
					if eat[e].user_global_affected > "" then 
						if _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] end
						_G["g_UserGlobal['"..eat[e].user_global_affected.."']"] = currentvalue[e] - eat[e].quantity
						if _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..eat[e].user_global_affected.."']"] = 0 end
						if eat[e].poisoned == 1 then
							CollisionOff(e)			
							Hide(e)
							if eat[e].respawning == 1 then respawntime[e] = g_Time + ((eat[e].respawn_time*1000)*60) end
							status[e] = "poisoned"
						end
						if eat[e].poisoned == 0 then
							CollisionOff(e)			
							Hide(e)
							if eat[e].respawning == 1 then respawntime[e] = g_Time + ((eat[e].respawn_time*1000)*60) end
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
			CollisionOff(e)			
			Hide(e)
			if eat[e].respawning == 1 then respawntime[e] = g_Time + ((eat[e].respawn_time*1000)*60) end
		end
	end
	
	if g_Time > respawntime[e] then
		if eat[e].respawning == 1 then
			CollisionOn(e)
			Show(e)
			pressed[e] = 0
			actioned[e] = 0
			status[e] = "init"
		end
		if eat[e].respawning == 0 and pressed[e] == 1 then
			pressed[e] = 0
			Destroy(e)
		end
	end
end