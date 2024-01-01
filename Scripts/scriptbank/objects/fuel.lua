-- Fuel v7
-- DESCRIPTION: The attached object will give the player a fuel resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [AMOUNT=5(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Collected Fuel"]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

local U = require "scriptbank\\utillib"
g_fuel = {}
local fuel 				= {}
local prompt_text 		= {}
local amount 			= {}
local pickup_range 		= {}
local pickup_style 		= {}
local collected_text 	= {}

local total 			= {}
local tEnt 				= {}
local selectobj 		= {}		

function fuel_properties(e, prompt_text, amount, pickup_range, pickup_style, collected_text)
	fuel[e] = g_Entity[e]
	fuel[e].prompt_text = prompt_text
	fuel[e].amount = amount
	fuel[e].pickup_range = pickup_range
	fuel[e].pickup_style = pickup_style
	fuel[e].collected_text = collected_text
end

function fuel_init_name(e)	
	fuel[e] = g_Entity[e]	
	fuel[e].prompt_text = "E to Collect"
	fuel[e].amount = 5
	fuel[e].pickup_range = 80
	fuel[e].pickup_style = 1
	fuel[e].collected_text = "Collected Fuel"
	g_fuel = 0
	tEnt[e] = 0
	selectobj[e] = 0
end

function fuel_main(e)
	fuel[e] = g_Entity[e]
	local PlayerDist = GetPlayerDistance(e)
	
	if fuel[e].pickup_style == 1 then
		if PlayerDist < fuel[e].pickup_range then	
			Prompt(fuel[e].collected_text)
			PlaySound(e,0)
			PerformLogicConnections(e)		
			g_fuel = g_fuel + fuel[e].amount		
			Destroy(e)
		end
	end
	
	if fuel[e].pickup_style == 2 then
		if PlayerDist < fuel[e].pickup_range then
			-- pinpoint select object--
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,fuel[e].pickup_range
			local paX, paY, paZ = math.rad(GetCameraAngleX(0)), math.rad(GetCameraAngleY(0)), math.rad(GetCameraAngleZ(0))
			rayX, rayY, rayZ = U.Rotate3D(rayX, rayY, rayZ, paX, paY, paZ)
			selectobj[e]=IntersectAll(px,py,pz, px+rayX, py+rayY, pz+rayZ,e)
			if selectobj[e] ~= 0 or selectobj[e] ~= nil then
				if g_Entity[e]['obj'] == selectobj[e] then
					TextCenterOnXColor(50-0.01,50,3,"+",255,255,255) --highliting (with crosshair at present)
					tEnt[e] = e
				end
			end
			if selectobj[e] == 0 or selectobj[e] == nil then
				tEnt[e] = 0
				TextCenterOnXColor(50-0.01,50,3,"+",155,155,155) --highliting (with crosshair at present)
			end
			--end pinpoint select object--
		end
		if PlayerDist < fuel[e].pickup_range and tEnt[e] ~= 0 and tEnt[e] ~= nil then	
			PromptLocal(e,fuel[e].prompt_text)
			if g_KeyPressE == 1 then				
				PlaySound(e,0)
				PerformLogicConnections(e)		
				g_fuel = g_fuel + fuel[e].amount
				Prompt(fuel[e].collected_text)
				Destroy(e)
			end
		end
	end
end
