-- DESCRIPTION: The object will give the player a food health boost or deduction if consumed.
-- Food v15
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Food consumed"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyGlobal
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> for useage sound.
-- DESCRIPTION: <Sound1> for collection sound.
-- DESCRIPTION: <Sound2> for poisoning sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local food					= {}
local prompt_text			= {}
local prompt_if_collectable = {}
local useage_text			= {}
local quantity				= {}
local pickup_range			= {}
local pickup_style			= {}
local effect				= {}
local user_global_affected	= {}
local prompt_display 		= {}
local item_highlight 		= {}

local use_item_now = {}
local tEnt = {}
local selectobj = {}

function food_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight)
	food[e].prompt_text = prompt_text
	food[e].prompt_if_collectable = prompt_if_collectable
	food[e].useage_text = useage_text
	food[e].quantity = quantity
	food[e].pickup_range = pickup_range
	food[e].pickup_style = pickup_style
	food[e].effect = effect
	food[e].user_global_affected = user_global_affected
	food[e].prompt_display = prompt_display
	food[e].item_highlight = item_highlight
end

function food_init(e)
	food[e] = {}
	food[e].prompt_text = "E to Use"
	food[e].prompt_if_collectable = "E to collect"
	food[e].useage_text = "food consumed"
	food[e].quantity = 10
	food[e].pickup_range = 80
	food[e].pickup_style = 1
	food[e].effect = 1
	food[e].user_global_affected = "MyMana"
	food[e].prompt_display = 1
	food[e].item_highlight = 0
	
	use_item_now[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
end

function food_main(e)

	PlayerDist = GetPlayerDistance(e)	
	if food[e].pickup_style == 1 then
		if PlayerDist < food[e].pickup_range then
			if food[e].prompt_display == 1 then PromptLocal(e,food[e].useage_text) end
			if food[e].prompt_display == 2 then Prompt(food[e].useage_text) end
			use_item_now[e] = 1
		end
	end
	if food[e].pickup_style == 2 and PlayerDist < food[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,food[e].pickup_range,food[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if PlayerDist < food[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if food[e].prompt_display == 1 then PromptLocal(e,food[e].prompt_text) end
				if food[e].prompt_display == 2 then Prompt(food[e].prompt_text) end			
				if g_KeyPressE == 1 then				
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if food[e].prompt_display == 1 then PromptLocal(e,food[e].prompt_if_collectable) end
				if food[e].prompt_display == 2 then Prompt(food[e].prompt_if_collectable) end			
				-- if collectable or resource
				if g_KeyPressE == 1 then
					Hide(e)
					CollisionOff(e)
					SetEntityCollected(tEnt[e],1)
					PlaySound(e,1)
				end
			end
		end
	end
	local tusedvalue = GetEntityUsed(e)
	if tusedvalue > 0 then
		-- if this is a resource, it will deplete qty and set used to zero
		if food[e].prompt_display == 1 then PromptLocal(e,food[e].useage_text) end
		if food[e].prompt_display == 2 then Prompt(food[e].useage_text) end			
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if food[e].effect == 1 then addquantity = 1 end
		if food[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end
	
	local currentvalue = 0
	if addquantity == 1 then
		if food[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..food[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..food[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..food[e].user_global_affected.."']"] = currentvalue + food[e].quantity
			if _G["g_UserGlobal['"..food[e].user_global_affected.."']"] >= 100 then _G["g_UserGlobal['"..food[e].user_global_affected.."']"] = 100 end
			SetPlayerHealth(g_PlayerHealth + food[e].quantity)
			if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
			SetPlayerHealthCore(g_PlayerHealth)
		end
	end
	if addquantity == 2 then
		if food[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..food[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..food[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..food[e].user_global_affected.."']"] = currentvalue - food[e].quantity
			if _G["g_UserGlobal['"..food[e].user_global_affected.."']"] <= 0 then _G["g_UserGlobal['"..food[e].user_global_affected.."']"] = 0 end
		end
		SetPlayerHealth(g_PlayerHealth - food[e].quantity)
		SetPlayerHealthCore(g_PlayerHealth)
		if g_PlayerHealth <= 0 then g_PlayerHealth = 0 end
		PlaySound(e,2)
	end
end
