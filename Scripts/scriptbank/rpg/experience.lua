-- DESCRIPTION: The object will give the player an experience addition or deduction if used.
-- Experience v14
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USAGE_TEXT$="Experience consumed"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyXP"] User global to be affected (eg: MyXP)
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> for usage sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local experience = {}
local prompt_text = {}
local prompt_if_collectable = {}
local usage_text, quantity = {}
local pickup_range = {}
local pickup_style = {}
local effect = {}
local user_global_affected = {}
local prompt_display = {}
local item_highlight = {}

local use_item_now = {}
local tEnt = {}
local selectobj = {}

function experience_properties(e, prompt_text, prompt_if_collectable, usage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight)
	experience[e].prompt_text = prompt_text
	experience[e].prompt_if_collectable = prompt_if_collectable
	experience[e].usage_text = usage_text
	experience[e].quantity = quantity
	experience[e].pickup_range = pickup_range
	experience[e].pickup_style = pickup_style
	experience[e].effect = effect
	experience[e].user_global_affected = user_global_affected
	experience[e].prompt_display = prompt_display
	experience[e].item_highlight = item_highlight	
end

function experience_init(e)
	experience[e] = {}
	experience[e].prompt_text = "E to consume"
	experience[e].prompt_if_collectable = "E to collect"
	experience[e].usage_text = "Experience consumed"
	experience[e].quantity = 10
	experience[e].pickup_range = 80
	experience[e].pickup_style = 1
	experience[e].effect = 1
	experience[e].user_global_affected = ""
	experience[e].prompt_display = 1
	experience[e].item_highlight = 0 
	
	use_item_now[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
end

function experience_main(e)

	local PlayerDist = GetPlayerDistance(e)	
	if experience[e].pickup_style == 1 then
		if PlayerDist < experience[e].pickup_range then
			Prompt(experience[e].usage_text)
			use_item_now[e] = 1
		end
	end
	if experience[e].pickup_style == 2 and PlayerDist < experience[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,experience[e].pickup_range,experience[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		
		if PlayerDist < experience[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then		
			if GetEntityCollectable(tEnt[e]) == 0 then
				if experience[e].prompt_display == 1 then PromptLocal(e,experience[e].prompt_text) end
				if experience[e].prompt_display == 2 then Prompt(experience[e].prompt_text) end
				if g_KeyPressE == 1 then
					if experience[e].prompt_display == 1 then PromptLocal(e,experience[e].usage_text) end
					if experience[e].prompt_display == 2 then Prompt(experience[e].usage_text) end					
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if experience[e].prompt_display == 1 then PromptLocal(e,experience[e].prompt_if_collectable) end
				if experience[e].prompt_display == 2 then Prompt(experience[e].prompt_if_collectable) end
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
		PromptDuration(experience[e].usage_text,2000)		
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if experience[e].effect == 1 then addquantity = 1 end
		if experience[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end
	local currentvalue = 0
	if addquantity == 1 then
		if experience[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..experience[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..experience[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..experience[e].user_global_affected.."']"] = currentvalue + experience[e].quantity
		end
	end
	if addquantity == 2 then
		if experience[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..experience[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..experience[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..experience[e].user_global_affected.."']"] = currentvalue - experience[e].quantity
		end
	end
end
