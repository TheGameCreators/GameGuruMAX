-- DESCRIPTION: The object will give the player a potion boost or deduction if consumed.
-- Potion v14
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Potion consumed"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMana"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> for useage sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local potion = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local effect = {}
local user_global_affected = {}
local prompt_display = {}
local item_highlight = {}
local use_item_now = {}
local tEnt = {}
local selectobj = {}

function potion_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight)
	potion[e].prompt_text = prompt_text
	potion[e].prompt_if_collectable = prompt_if_collectable
	potion[e].useage_text = useage_text
	potion[e].quantity = quantity
	potion[e].pickup_range = pickup_range
	potion[e].pickup_style = pickup_style
	potion[e].effect = effect
	potion[e].user_global_affected = user_global_affected
	potion[e].prompt_display = prompt_display
	potion[e].item_highlight = item_highlight	
end

function potion_init(e)
	potion[e] = {}
	potion[e].prompt_text = "E to Use"
	potion[e].prompt_if_collectable = "E to collect"
	potion[e].useage_text = "Potion consumed"
	potion[e].quantity = 10
	potion[e].pickup_range = 80
	potion[e].pickup_style = 1
	potion[e].effect = 1
	potion[e].user_global_affected = "MyMana"
	potion[e].prompt_display = 1
	potion[e].item_highlight = 0	
	use_item_now[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
end

function potion_main(e)

	PlayerDist = GetPlayerDistance(e)	
	if potion[e].pickup_style == 1 then
		if PlayerDist < potion[e].pickup_range then
			if potion[e].prompt_display == 1 then PromptLocal(e,potion[e].useage_text) end
			if potion[e].prompt_display == 2 then Prompt(potion[e].useage_text) end
			use_item_now[e] = 1
		end
	end
	if potion[e].pickup_style == 2 and PlayerDist < potion[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,potion[e].pickup_range,potion[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if PlayerDist < potion[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if potion[e].prompt_display == 1 then PromptLocal(e,potion[e].prompt_text) end
				if potion[e].prompt_display == 2 then Prompt(potion[e].prompt_text) end			
				if g_KeyPressE == 1 then				
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if potion[e].prompt_display == 1 then PromptLocal(e,potion[e].prompt_if_collectable) end
				if potion[e].prompt_display == 2 then Prompt(potion[e].prompt_if_collectable) end
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
		if potion[e].prompt_display == 1 then PromptLocal(e,potion[e].useage_text) end
		if potion[e].prompt_display == 2 then Prompt(potion[e].useage_text) end		
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if potion[e].effect == 1 then addquantity = 1 end
		if potion[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
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
