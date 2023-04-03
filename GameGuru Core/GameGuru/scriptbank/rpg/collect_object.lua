-- Collect Object v3
-- DESCRIPTION: Will allow collection of an object. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [COLLECTED_TEXT$="Item collected"]
-- DESCRIPTION: <Sound0> for collection sound.

local U = require "scriptbank\\utillib"
local P = require "scriptbank\\physlib"
local collect_object = {}
local tEnt = {}
local selectobj = {}

function collect_object_properties(e, pickup_text, pickup_range, collected_text)
	collect_object[e].pickup_text = pickup_text
	collect_object[e].pickup_range = pickup_range	
	collect_object[e].collected_text = collected_text
end

function collect_object_init(e)
	collect_object[e] = {}
	collect_object[e].pickup_text = "E to collect"
	collect_object[e].pickup_range = 80
	collect_object[e].collected_text = "Item collected"
	tEnt[e] = 0
	selectobj[e] = 0
end

function collect_object_main(e)
	PlayerDist = GetPlayerDistance(e)	
	--pinpoint select object--
	local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
	local rayX, rayY, rayZ = 0,0,collect_object[e].pickup_range
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
	
	if PlayerDist < collect_object[e].pickup_range and tEnt[e] ~= 0 or nil and GetEntityVisibility(e) == 1 then
		if GetEntityCollectable(tEnt[e]) == 1 then
			if GetEntityCollected(tEnt[e]) == 0 then
				Prompt(collect_object[e].pickup_text)
				if g_KeyPressE == 1 then				
					Prompt(collect_object[e].collected_text)
					PerformLogicConnections(e)
					SetEntityCollected(tEnt[e],1)
					PlaySound(e,0)
				end
			end
		end
	end
end
