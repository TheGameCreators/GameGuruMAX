-- Bottled_Item v9  by Necrym59
-- DESCRIPTION: The object will give the player a health boost or loss if consumed.
-- DESCRIPTION: and can also effect a user global if required.
-- DESCRIPTION: [PROMPT_TEXT$="Press E to consume"]
-- DESCRIPTION: [QUANTITY=10(1,40)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$=""] "MyGlobal" for example.
-- DESCRIPTION: [POISONING_EFFECT!=0]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> when consuming.
-- DESCRIPTION: <Sound1> when poisoned.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local bottle 				= {}
local prompt_text 			= {}
local quantity 				= {}
local pickup_range 			= {}
local effect 				= {}
local poisoning_effect		= {}
local user_global_affected 	= {}
local prompt_display		= {}
local item_highlight 		= {}

local tEnt 					= {}
local selectobj 			= {}
local currentvalue 			= {}
local addquantity 			= {}
local calchealth	 		= {}
local poisoned				= {}
local doonce				= {}
local actioned				= {}
local status 				= {}

function bottled_item_properties(e, prompt_text, quantity, pickup_range, effect, user_global_affected, poisoning_effect, prompt_display, item_highlight)
	bottle[e].prompt_text = prompt_text
	bottle[e].quantity = quantity
	bottle[e].pickup_range = pickup_range
	bottle[e].effect = effect
	bottle[e].user_global_affected = user_global_affected
	bottle[e].poisoning_effect = poisoning_effect
	bottle[e].prompt_display = prompt_display
	bottle[e].item_highlight = item_highlight
end

function bottled_item_init(e)
	bottle[e] = {}
	bottle[e].prompt_text = "Press E to consume"
	bottle[e].quantity = 10
	bottle[e].pickup_range = 80
	bottle[e].effect = 1
	bottle[e].user_global_affected = "MyHealth"
	bottle[e].poisoning_effect = 0
	bottle[e].prompt_display = 1
	bottle[e].item_highlight = 0

	currentvalue[e] = 0
	addquantity[e] = 0
	calchealth[e] = 0
	poisoned[e] = 0
	doonce[e] = 0
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	actioned[e] = 0
	status[e] = "init"
end

function bottled_item_main(e)

	if status[e] == "init" then
		if bottle[e].effect == 1 then addquantity[e] = 1 end
		if bottle[e].effect == 2 then addquantity[e] = 2 end
		poisoned[e] = bottle[e].quantity * 30
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < bottle[e].pickup_range and GetEntityVisibility(e) == 1 then
		--pinpoint select object--
		module_misclib.pinpoint(e,bottle[e].pickup_range,bottle[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < bottle[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if doonce[e] == 0 then
				if bottle[e].prompt_display == 1 then PromptLocal(e,bottle[e].prompt_text) end
				if bottle[e].prompt_display == 2 then Prompt(bottle[e].prompt_text) end
			end
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
						if bottle[e].poisoning_effect == 1 then
							Hide(e)
							CollisionOff(e)
							status[e] = "poisoned"
						end
						if bottle[e].poisoning_effect == 0 then
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