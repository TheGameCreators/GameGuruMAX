-- Resource v5
-- DESCRIPTION: The object will give the player a crafting resource. Object must be set to 'Collectable' and 'Resource'.
-- DESCRIPTION: [PICKUP_TEXT$="E to harvest"]
-- DESCRIPTION: [COLLECTED_TEXT$="Resource harvested"]
-- DESCRIPTION: [QUANTITY=1(1,5)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: <Sound0> for harvesting sound.

local U = require "scriptbank\\utillib"

g_ResnodeCollected = {}
local resource = {}
local pickup_text = {}
local collected_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local tEnt = {}
local selectobj = {}

function resource_properties(e, pickup_text, collected_text, quantity, pickup_range, pickup_style)
	resource[e] = g_Entity[e]
	resource[e].pickup_text = pickup_text
	resource[e].collected_text = collected_text
	resource[e].quantity = quantity
	resource[e].pickup_range = pickup_range
	resource[e].pickup_style = pickup_style
end

function resource_init(e)
	resource[e] = {}
	resource[e].pickup_text = "E to harvest"
	resource[e].collected_text = "Resource harvested"
	resource[e].quantity = 1
	resource[e].pickup_range = 80
	resource[e].pickup_style = 1
	SetEntityQuantity(e,resource[e].quantity)
	tEnt[e] = 0
	selectobj[e] = 0
end

function resource_main(e)
	local PlayerDist = GetPlayerDistance(e)	
	
	if resource[e].pickup_style == 1 and PlayerDist < resource[e].pickup_range then
		if GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then
				Prompt(resource[e].pickup_text)
				Prompt(resource[e].collected_text)
				PerformLogicConnections(e)
				PlaySound(e,0)
				SetEntityCollected(e,1,-1)
				g_ResnodeCollected = e				
			end
		end
	end
	
	if resource[e].pickup_style == 2 and PlayerDist < resource[e].pickup_range then
		-- pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,resource[e].pickup_range
		local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
		rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
		selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
		if selectobj[e] ~= 0 or selectobj[e] ~= nil then
			if g_Entity[e]['obj'] == selectobj[e] then
				TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
				tEnt[e] = e
			else
				tEnt[e] = 0				
			end
		end
		if selectobj[e] == 0 or selectobj[e] == nil then
			tEnt[e] = 0
			TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
		end
		--end pinpoint select object--	
		
		if PlayerDist < resource[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(e) == 2 then
				if GetEntityCollected(e) == 0 then
					Prompt(resource[e].pickup_text)
					if g_KeyPressE == 1 then
						Prompt(resource[e].collected_text)
						PerformLogicConnections(e)
						PlaySound(e,0)
						SetEntityCollected(e,1,-1)
						g_ResnodeCollected = e
					end
				end
			end
		end
	end	
end
