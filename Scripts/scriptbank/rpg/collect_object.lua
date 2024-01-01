-- Collect Object v8
-- DESCRIPTION: Will allow collection of an object. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Item collected"]
-- DESCRIPTION: <Sound0> for collection sound.

local U = require "scriptbank\\utillib"

g_ResnodeCollected = {}
local collect_object = {}
local pickup_text = {}
local pickup_range = {}
local pickup_style = {}
local collected_text = {}

local tEnt = {}
local selectobj = {}

function collect_object_properties(e, pickup_text, pickup_range,  pickup_style, collected_text)
	collect_object[e].pickup_text = pickup_text
	collect_object[e].pickup_range = pickup_range
	collect_object[e].pickup_style = pickup_style
	collect_object[e].collected_text = collected_text
end

function collect_object_init(e)
	collect_object[e] = {}
	collect_object[e].pickup_text = "E to collect"
	collect_object[e].pickup_range = 80
	collect_object[e].pickup_style = 2
	collect_object[e].collected_text = "Item collected"
	tEnt[e] = 0
	selectobj[e] = 0
end

function collect_object_main(e)
	local PlayerDist = GetPlayerDistance(e)
	
	if collect_object[e].pickup_style == 1 and PlayerDist < collect_object[e].pickup_range then
		if GetEntityCollectable(e) == 1 or GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then					
				Prompt(collect_object[e].collected_text)
				PerformLogicConnections(e)
				SetEntityCollected(e,1)
				g_ResnodeCollected = e
				PlaySound(e,0)
			end
		end
	end
	
	if collect_object[e].pickup_style == 2 and PlayerDist < collect_object[e].pickup_range then
		--pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,collect_object[e].pickup_range
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
	end
	if PlayerDist < collect_object[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
		if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
			if GetEntityCollected(tEnt[e]) == 0 then
				Prompt(collect_object[e].pickup_text)
				if g_KeyPressE == 1 then				
					Prompt(collect_object[e].collected_text)
					PerformLogicConnections(e)
					SetEntityCollected(tEnt[e],1)
					g_ResnodeCollected = e
					PlaySound(e,0)
				end
			end
		end
	end
end
