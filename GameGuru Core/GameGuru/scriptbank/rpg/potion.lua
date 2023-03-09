-- DESCRIPTION: The object will give the player a potion boost or deduction if consumed.
-- Potion v7
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Potion consumed"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: <Sound0> for collection sound.

local potion = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local effect = {}
local user_global_affected = {}
local use_item_now = {}

function potion_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, user_global_affected)
	potion[e] = g_Entity[e]	
	potion[e].prompt_text = prompt_text
	potion[e].prompt_if_collectable = prompt_if_collectable
	potion[e].useage_text = useage_text
	potion[e].quantity = quantity
	potion[e].pickup_range = pickup_range
	potion[e].pickup_style = pickup_style
	potion[e].effect = effect
	potion[e].user_global_affected = user_global_affected
end

function potion_init(e)
	potion[e] = g_Entity[e]
	potion[e].prompt_text = "E to Use"
	potion[e].prompt_if_collectable = "E to collect"
	potion[e].useage_text = "Potion consumed"
	potion[e].quantity = 10
	potion[e].pickup_range = 80
	potion[e].pickup_style = 1
	potion[e].effect = 1
	potion[e].user_global_affected = "MyMana"
	use_item_now[e] = 0
end

function potion_main(e)
	potion[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)	
	if potion[e].pickup_style == 1 then
		if PlayerDist < potion[e].pickup_range then
			PromptDuration(potion[e].useage_text,1000)
			use_item_now[e] = 1
		end
	end
	if potion[e].pickup_style == 2 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < potion[e].pickup_range then
			if GetEntityCollectable(e) == 0 then				
				PromptDuration(potion[e].prompt_text,1000)
				if g_KeyPressE == 1 then				
					use_item_now[e] = 1
				end
			else
				if GetEntityCollected(e) == 0 then
					Prompt(potion[e].prompt_if_collectable)
					if g_KeyPressE == 1 then
						SetEntityCollected(e,1,-1)
					end
				end
			end
		end
	end
	local tusedvalue = GetEntityUsed(e)
	if tusedvalue > 0 then
		PromptDuration(potion[e].useage_text,1000)
		use_item_now[e] = 1
		SetEntityUsed(e,tusedvalue*-1)
	end
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if potion[e].effect == 1 then addquantity = 1 end
		if potion[e].effect == 2 then addquantity = 2 end
		Destroy(e)
	end
	local currentvalue = 0
	if addquantity == 1 then
		if potion[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..potion[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..potion[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..potion[e].user_global_affected.."']"] = currentvalue + potion[e].quantity
		end
	end
	if addquantity == 2 then
		if potion[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..potion[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..potion[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..potion[e].user_global_affected.."']"] = currentvalue - potion[e].quantity
		end
	end
end
