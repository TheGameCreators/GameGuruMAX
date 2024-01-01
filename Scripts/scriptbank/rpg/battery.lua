-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Battery v6: by Necrym59
-- DESCRIPTION: The attached object will give the player a battery energy resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Battery collected"]
-- DESCRIPTION: [ENERGY_LEVEL=10(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=90(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyBatteryEnergy"]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

local U = require "scriptbank\\utillib"
g_batteryenergy = {}
local battery 				= {}
local prompt_text 			= {}
local collected_text 		= {}
local energy_level 			= {}
local pickup_range 			= {}	
local pickup_style 			= {}
local user_global_affected	= {}

local currentvalue 		= {}
local pressed 			= {}
local played 			= {}
local status			= {}
local tEnt				= {}
local selectobj			= {}		

function battery_properties(e, prompt_text, collected_text, energy_level, pickup_range, pickup_style, user_global_affected)
	battery[e] = g_Entity[e]
	battery[e].prompt_text = prompt_text
	battery[e].collected_text = collected_text
	battery[e].energy_level = energy_level
	battery[e].pickup_range = pickup_range
	battery[e].pickup_style = pickup_style
	battery[e].user_global_affected = user_global_affected	
	battery[e].pickup_pointer = "imagebank\\cursors\\cross.png"
end

function battery_init_name(e)	
	battery[e] = {}	
	battery[e].prompt_text = "E to collect"
	battery[e].collected_text = "Collected battery"
	battery[e].energy_level = 0
	battery[e].pickup_range = 50
	battery[e].pickup_style = pickup_style
	battery[e].user_global_affected = "MyBatteryEnergy"	
	g_batteryenergy = 0
	played[e] = 0
	pressed[e] = 0
	currentvalue[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
	status[e] = "init"	
end

function battery_main(e)
	battery[e] = g_Entity[e]
	
	if status[e] == "init" then
		status[e] = "endinit"
	end
	
	local PlayerDist = GetPlayerDistance(e)

	if battery[e].pickup_style == 1 and PlayerDist < battery[e].pickup_range then
		Prompt(battery[e].collected_text)
		if played[e] == 0 then
			PlaySound(e,0)
			played[e] = 1
		end
		PerformLogicConnections(e)		
		g_batteryenergy = g_batteryenergy + battery[e].energy_level
		Hide(e)
		CollisionOff(e)
		Destroy(e)
		pressed[e] = 1
		tEnt[e] = 0
	end		
	
	if battery[e].pickup_style == 2 and PlayerDist < battery[e].pickup_range then
		--pinpoint select object--		
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,battery[e].pickup_range
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
	
	if PlayerDist < battery[e].pickup_range and tEnt[e] ~= 0 then
		PromptLocal(e,battery[e].prompt_text)
		if g_KeyPressE == 1 and pressed[e] == 0 then
			Prompt(battery[e].collected_text)
			PlaySound(e,0)
			PerformLogicConnections(e)		
			g_batteryenergy = g_batteryenergy + battery[e].energy_level
			if battery[e].user_global_affected > "" then
				if _G["g_UserGlobal['"..battery[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..battery[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..battery[e].user_global_affected.."']"] = currentvalue[e] + g_batteryenergy
			end
			Hide(e)
			CollisionOff(e)
			Destroy(e)
			pressed[e] = 1			
		end
	end	
end