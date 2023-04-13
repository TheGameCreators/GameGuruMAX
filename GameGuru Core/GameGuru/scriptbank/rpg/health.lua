-- DESCRIPTION: The object will give the player an health boost or deduction if used. Can be used as a resource.
-- Health v10
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Health applied"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: <Sound0> for use sound.
-- DESCRIPTION: <Sound1> for collection sound.

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"

local health = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local pickup_range = {}
local effect = {}
local selectobj = {}
local tEnt = {}

function health_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect)
	health[e].prompt_text = prompt_text
	health[e].prompt_if_collectable = prompt_if_collectable
	health[e].useage_text = useage_text
	health[e].quantity = quantity
	health[e].pickup_range = pickup_range
	health[e].pickup_style = pickup_style
	health[e].effect = effect
end

function health_init(e)
	health[e] = {}
	health[e].prompt_text = "E to Use"
	health[e].prompt_if_collectable = "E to collect"
	health[e].useage_text = "Health applied"
	health[e].quantity = 10
	health[e].pickup_range = 80
	health[e].pickup_style = 1
	health[e].effect = 1
	tEnt[e] = 0
	selectobj[e] = 0
end

function health_main(e)

	local use_item_now = 0
	
	PlayerDist = GetPlayerDistance(e)	
	if health[e].pickup_style == 1 then
		if PlayerDist < health[e].pickup_range then
			PromptDuration(health[e].useage_text,1000)			
			use_item_now = 1
		end
	end
	
	if health[e].pickup_style == 2 then
		--pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,health[e].pickup_range
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] ~= 0 or nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				Text(50,50,3,"+") --highliting (with crosshair at present)
				tEnt[e] = e
			else
				tEnt[e] = 0
			end
		end	
		--end pinpoint select object--
		
		if PlayerDist < health[e].pickup_range and tEnt[e] ~= 0 or nil then
			if GetEntityCollectable(tEnt[e]) == 0 then			
				PromptDuration(health[e].prompt_text,1000)
				if g_KeyPressE == 1 then
					use_item_now = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then				
				-- if collectable or resource
				PromptDuration(health[e].prompt_if_collectable,1000)
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
		SetEntityUsed(e,tusedvalue*-1)
		PromptDuration(health[e].useage_text,1000)
		use_item_now = 1
	end
	
	local addquantity = 0
	if use_item_now == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if health[e].effect == 1 then addquantity = 1 end
		if health[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
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
