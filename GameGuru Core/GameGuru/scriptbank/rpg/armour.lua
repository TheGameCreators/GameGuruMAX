-- DESCRIPTION: The object will give the player an armour boost or deduction if used.
-- Armour v9
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Armour worn"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyArmour"]
-- DESCRIPTION: <Sound0> for collection sound.

local U = require "scriptbank\\utillib"

local armour = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text, quantity = {}
local pickup_range = {}
local pickup_style = {}
local effect = {}
local user_global_affected = {}
local use_item_now = {}
local tEnt = {}
local selectobj = {}

function armour_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, user_global_affected)
	armour[e] = g_Entity[e]	
	armour[e].prompt_text = prompt_text
	armour[e].prompt_if_collectable = prompt_if_collectable
	armour[e].useage_text = useage_text
	armour[e].quantity = quantity
	armour[e].pickup_range = pickup_range
	armour[e].pickup_style = pickup_style
	armour[e].effect = effect
	armour[e].user_global_affected = user_global_affected
end

function armour_init(e)
	armour[e] = g_Entity[e]
	armour[e].prompt_text = "E to Use"
	armour[e].prompt_if_collectable = "E to collect"
	armour[e].useage_text = "Armour worn"
	armour[e].quantity = 10
	armour[e].pickup_range = 80
	armour[e].pickup_style = 1
	armour[e].effect = 1
	armour[e].user_global_affected = "MyArmour"
	use_item_now[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
end

function armour_main(e)
	armour[e] = g_Entity[e]
	PlayerDist = GetPlayerDistance(e)	
	if armour[e].pickup_style == 1 then
		if PlayerDist < armour[e].pickup_range then
			PromptDuration(armour[e].useage_text,1000)
			use_item_now[e] = 1
		end
	end
	if armour[e].pickup_style == 2 and PlayerDist < armour[e].pickup_range then
		--pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,armour[e].pickup_range
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
		if PlayerDist < armour[e].pickup_range and tEnt[e] ~= 0 or nil and GetEntityVisibility(e) == 1 then		
			if GetEntityCollectable(tEnt[e]) == 0 then
				PromptDuration(armour[e].prompt_text,1000)
				if g_KeyPressE == 1 then				
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				Prompt(armour[e].prompt_if_collectable)
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
		PromptDuration(armour[e].useage_text,2000)		
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if armour[e].effect == 1 then addquantity = 1 end
		if armour[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end
	local currentvalue = 0
	if addquantity == 1 then
		if armour[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..armour[e].user_global_affected.."']"] = currentvalue + armour[e].quantity
		end
	end
	if addquantity == 2 then
		if armour[e].user_global_affected > "" then 
			if _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..armour[e].user_global_affected.."']"] = currentvalue - armour[e].quantity
		end
	end
end
