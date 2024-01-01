-- Chest Portable v9  by Necrym59
-- DESCRIPTION: The attached object can be used as a portable chest.
-- DESCRIPTION: Set object as Collectable.
-- DESCRIPTION: [PICKUP_TEXT$="E to Pickup"] [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [USEAGE_TEXT$="Press Y to access"]
-- DESCRIPTION: [USEAGE_KEY$="Y"]
-- DESCRIPTION: When use key is pressed, will open the [CHEST_SCREEN$="HUD Screen 6"]
-- DESCRIPTION: using [chest_container$="chestportable"]
-- DESCRIPTION: <Sound0> for pickup

local U = require "scriptbank\\utillib"
local lower = string.lower

local chest_portable	= {}
local pickup_text		= {}
local pickup_range		= {}
local pickup_style		= {}
local useage_text		= {}
local chest_screen 		= {}
local chest_container 	= {}
local have_chest 		= {}
local selectobj 		= {}
local doonce 			= {}
local tEnt 				= {}
local played			= {}

function chest_portable_properties(e, pickup_text, pickup_range, pickup_style, useage_text, useage_key, chest_screen, chest_container)
	chest_portable[e] = g_Entity[e]
	chest_portable[e].pickup_text = pickup_text
	chest_portable[e].pickup_range = pickup_range
	chest_portable[e].pickup_style = pickup_style
	chest_portable[e].useage_text = useage_text
	chest_portable[e].useage_key = lower(useage_key)
	chest_portable[e].chest_screen = chest_screen
	chest_portable[e].chest_container = chest_container
end

function chest_portable_init(e)
	chest_portable[e] = {}
	chest_portable[e].pickup_text = "E to Pickup"
	chest_portable[e].pickup_range = 80
	chest_portable[e].pickup_style = 2
	chest_portable[e].useage_text = "Press Y to access"
	chest_portable[e].useage_key = "y"
	chest_portable[e].chest_screen = "HUD Screen 7"
	chest_portable[e].chest_container = "chestportable"
	have_chest[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
	doonce[e] = 0
	played[e] = 0
end

function chest_portable_main(e)

	PlayerDist = GetPlayerDistance(e)

	if chest_portable[e].pickup_style == 1 then
		if have_chest[e] == 0 then
			if GetEntityCollectable(e) == 1 then
				if PlayerDist < chest_portable[e].pickup_range and g_PlayerHealth > 0 and have_chest[e] == 0 then
					have_chest[e] = 1
					if played[e] == 0 then
						PlaySound(e,0)
						played[e] = 1
					end
					SetEntityCollected(e,1)
					SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
				end
			end
		end
		if have_chest[e] == 1 and doonce[e] == 0 then 
			PromptDuration(chest_portable[e].useage_text,2000)
			doonce[e] = 1
		end	
	end

	if chest_portable[e].pickup_style == 2 and PlayerDist < chest_portable[e].pickup_range then
		--pinpoint select object--
		local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
		local rayX, rayY, rayZ = 0,0,chest_portable[e].pickup_range
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
		if have_chest[e] == 0 then
			if GetEntityCollectable(tEnt[e]) == 1 then
				if PlayerDist < chest_portable[e].pickup_range and tEnt[e] ~= 0 then
					PromptLocalForVR(e,chest_portable[e].pickup_text)
					if g_KeyPressE == 1 then
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
						have_chest[e] = 1						
						SetEntityCollected(tEnt[e],1)					
						PromptDuration(chest_portable[e].useage_text,2000)
					end
				end
			end
		end
	end

	if have_chest[e] > 0 then
		if GetEntityCollected(e) == 1 then
			SetPosition(e,g_PlayerPosX,g_PlayerPosY+1000,g_PlayerPosZ)
			if GetCurrentScreen() == -1 then
				-- in the game
				if GetInKey() == chest_portable[e].useage_key then 
					g_UserGlobalContainer = chest_portable[e].chest_container
					ScreenToggle(chest_portable[e].chest_screen)					
				end
			else
				-- in chest screen
			end
		end
	end
end