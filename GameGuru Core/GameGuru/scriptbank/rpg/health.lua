-- DESCRIPTION: The object will give the player an health boost or deduction if used.
-- Health v7
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Health applied"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: <Sound0> for collection sound.

local health = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local pickup_range = {}
local effect = {}
local use_item_now = {}

function health_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect)
	health[e] = g_Entity[e]	
	health[e].prompt_text = prompt_text
	health[e].prompt_if_collectable = prompt_if_collectable
	health[e].useage_text = useage_text
	health[e].quantity = quantity
	health[e].pickup_range = pickup_range
	health[e].pickup_style = pickup_style
	health[e].effect = effect
end

function health_init(e)
	health[e] = g_Entity[e]
	health[e].prompt_text = "E to Use"
	health[e].prompt_if_collectable = "E to collect"
	health[e].useage_text = "Health applied"
	health[e].quantity = 10
	health[e].pickup_range = 80
	health[e].pickup_style = 1
	health[e].effect = 1
	use_item_now[e] = 0
end

function health_main(e)
	health[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)	
	if health[e].pickup_style == 1 then
		if PlayerDist < health[e].pickup_range then
			PromptDuration(health[e].useage_text,1000)			
			use_item_now[e] = 1
		end
	end
	if health[e].pickup_style == 2 then
		local LookingAt = GetPlrLookingAtEx(e,1)
		if LookingAt == 1 and PlayerDist < health[e].pickup_range then
			if GetEntityCollectable(e) == 0 then				
				PromptDuration(health[e].prompt_text,1000)
				if g_KeyPressE == 1 then
					use_item_now[e] = 1
				end
			else
				if GetEntityCollected(e) == 0 then
					Prompt(health[e].prompt_if_collectable)
					if g_KeyPressE == 1 then
						Hide(e)
						CollisionOff(e)
						SetEntityCollected(e,1)
					end
				end
			end
		end
	end
	
	local tusedvalue = GetEntityUsed(e)
	if tusedvalue > 0 then
		PromptDuration(health[e].useage_text,1000)
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if health[e].effect == 1 then addquantity = 1 end
		if health[e].effect == 2 then addquantity = 2 end
		Destroy(e)
	end
	
	local currentvalue = 0
	if addquantity == 1 then
		if g_PlayerHealth + health[e].quantity > g_PlayerStartStrength then
			SetPlayerHealth(g_PlayerStartStrength)
		else
			SetPlayerHealth(g_PlayerHealth + health[e].quantity)
		end		
		if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
	end
	if addquantity == 2 then
		SetPlayerHealth(g_PlayerHealth - health[e].quantity)
	end
	
end
