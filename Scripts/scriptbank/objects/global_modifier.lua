-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Global Modifier v3: by Necrym59
-- DESCRIPTION: The attached object will pass a temporary global value resource for use with other behaviors, such as countedowns or monitors.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Modifier collected"]
-- DESCRIPTION: [MODIFIER_LEVEL=10(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=90(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [USER_GLOBAL_AFFECTED$="MyGlobalModifier"]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [HEALTH_MODIFIER=10(1,30)]
-- DESCRIPTION: Play the audio <Sound0> when picked up

local U = require "scriptbank\\utillib"

local modifier 				= {}
local prompt_text 			= {}
local collected_text 		= {}
local modifier_level 		= {}
local pickup_range 			= {}
local pickup_style 			= {}
local user_global_affected	= {}
local effect				= {}
local health_modifier		= {}

local currentvalue 		= {}
local pressed 			= {}
local played 			= {}
local status			= {}
local tEnt				= {}
local selectobj			= {}

function global_modifier_properties(e, prompt_text, collected_text, modifier_level, pickup_range, pickup_style, user_global_affected, effect, health_modifier)
	modifier[e] = g_Entity[e]
	modifier[e].prompt_text = prompt_text
	modifier[e].collected_text = collected_text
	modifier[e].modifier_level = modifier_level
	modifier[e].pickup_range = pickup_range
	modifier[e].pickup_style = pickup_style
	modifier[e].user_global_affected = user_global_affected
	modifier[e].effect = effect
	modifier[e].health_modifier = health_modifier
end

function global_modifier_init(e)
	modifier[e] = {}
	modifier[e].prompt_text = "E to collect"
	modifier[e].collected_text = "Collected modifier"
	modifier[e].modifier_level = 0
	modifier[e].pickup_range = 50
	modifier[e].pickup_style = pickup_style
	modifier[e].user_global_affected = "MyModifier"
	modifier[e].effect = 1
	modifier[e].health_modifier = 0

	played[e] = 0
	pressed[e] = 0
	currentvalue[e] = 0
	tEnt[e] = 0
	selectobj[e] = 0
	status[e] = "init"
end

function global_modifier_main(e)

	if status[e] == "init" then
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if modifier[e].pickup_style == 1 and PlayerDist < modifier[e].pickup_range then
		Prompt(modifier[e].collected_text)
		if played[e] == 0 then
			PlaySound(e,0)
			played[e] = 1
		end
		PerformLogicConnections(e)
		if modifier[e].user_global_affected > "" and modifier[e].effect == 1 then
			if _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] = currentvalue[e] + modifier[e].modifier_level
			SetPlayerHealth(g_PlayerHealth + modifier[e].health_modifier)
		end
		if modifier[e].user_global_affected > "" and modifier[e].effect == 2 then
			if _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] = currentvalue[e] - modifier[e].modifier_level
			SetPlayerHealth(g_PlayerHealth - modifier[e].health_modifier)
		end
		Hide(e)
		CollisionOff(e)
		Destroy(e)
		pressed[e] = 1
		tEnt[e] = 0
	end

	if modifier[e].pickup_style == 2 and PlayerDist < modifier[e].pickup_range then
		--pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,modifier[e].pickup_range
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

	if PlayerDist < modifier[e].pickup_range and tEnt[e] ~= 0 then
		PromptLocal(e,modifier[e].prompt_text)
		if g_KeyPressE == 1 and pressed[e] == 0 then
			Prompt(modifier[e].collected_text)
			PlaySound(e,0)
			PerformLogicConnections(e)
			if modifier[e].user_global_affected > "" and modifier[e].effect == 1 then
				if _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] = currentvalue[e] + modifier[e].modifier_level
				SetPlayerHealth(g_PlayerHealth + modifier[e].health_modifier)
			end
			if modifier[e].user_global_affected > "" and modifier[e].effect == 2 then
				if _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] end
				_G["g_UserGlobal['"..modifier[e].user_global_affected.."']"] = currentvalue[e] - modifier[e].modifier_level
				SetPlayerHealth(g_PlayerHealth - modifier[e].health_modifier)
			end
			Hide(e)
			CollisionOff(e)
			Destroy(e)
			pressed[e] = 1
		end
	end
end