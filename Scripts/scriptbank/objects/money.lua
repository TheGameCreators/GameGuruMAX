-- Money v4
-- DESCRIPTION: The object will give the player money.
-- DESCRIPTION: [PICKUP_TEXT$="E to Collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Money gained"]
-- DESCRIPTION: [QUANTITY=10(1,500)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyMoney"]
-- DESCRIPTION: <Sound0> for collection sound.

local U = require "scriptbank\\utillib"
local money = {}
local pickup_text = {}
local collected_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local tEnt = {}
local selectobj = {}
local played = {}
local currentvalue = {}

function money_properties(e, pickup_text, collected_text, quantity, pickup_range, pickup_style, user_global_affected)
	money[e] = g_Entity[e]
	money[e].pickup_text = pickup_text
	money[e].collected_text = collected_text
	money[e].quantity = quantity
	money[e].pickup_range = pickup_range
	money[e].pickup_style = pickup_style
	money[e].user_global_affected = "MyMoney"
end

function money_init(e)
	money[e] = {}
	money[e].pickup_text = "E to harvest"
	money[e].collected_text = "money harvested"
	money[e].quantity = 1
	money[e].pickup_range = 80
	money[e].pickup_style = 1
	money[e].user_global_affected = "MyMoney"
	currentvalue[e] = 0
	played[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
end

function money_main(e)
	local PlayerDist = GetPlayerDistance(e)	
	
	if money[e].pickup_style == 1 and PlayerDist < money[e].pickup_range then
		Prompt(money[e].collected_text)
		PerformLogicConnections(e)
		if played[e] == 0 then
			PlaySound(e,0)
			played[e] = 1
		end
		if money[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..money[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..money[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..money[e].user_global_affected.."']"] = currentvalue[e] + money[e].quantity
		end	
		Destroy(e)
		tEnt[e] = 0		
	end
	
	if money[e].pickup_style == 2 and PlayerDist < money[e].pickup_range then
		--pinpoint select object--		
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,money[e].pickup_range
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
		
		if PlayerDist < money[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			Prompt(money[e].pickup_text)
			if g_KeyPressE == 1 then
				Prompt(money[e].collected_text)
				PerformLogicConnections(e)
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if money[e].user_global_affected > "" then
					if _G["g_UserGlobal['"..money[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..money[e].user_global_affected.."']"] end
					_G["g_UserGlobal['"..money[e].user_global_affected.."']"] = currentvalue[e] + money[e].quantity
				end	
				Destroy(e)
				tEnt[e] = 0				
			end
		end
	end	
end
