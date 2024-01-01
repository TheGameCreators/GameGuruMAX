-- Crafting Table v8
-- DESCRIPTION: The attached object can be used as a crafting table.
-- DESCRIPTION: When player is within [USE_RANGE=100] distance,
-- DESCRIPTION: show [USE_PROMPT$="Press E to begin crafting"] and
-- DESCRIPTION: when use key is pressed, will open the [CRAFT_SCREEN$="HUD Screen 7"]
-- DESCRIPTION: using [CRAFT_CONTAINER$="chest"]
-- DESCRIPTION: <Sound0> when crafting started.

local U = require "scriptbank\\utillib"
local crafting_table 	= {}
local use_range 		= {}
local use_prompt 		= {}
local craft_screen 		= {}
local craft_container 	= {}
local tEnt 				= {}
local selectobj			= {}
local played			= {}

function crafting_table_properties(e, use_range, use_prompt, craft_screen, craft_container)
	crafting_table[e].use_range = use_range
	crafting_table[e].use_prompt = use_prompt
	crafting_table[e].craft_screen = craft_screen
	crafting_table[e].craft_container = craft_container
end

function crafting_table_init(e)
	crafting_table[e] = {}
	crafting_table[e].use_range = 100
	crafting_table[e].use_prompt = "Press E to begin crafting"
	crafting_table[e].craft_screen = "HUD Screen 7"
	crafting_table[e].craft_container = "chest"
	tEnt[e] = 0
	selectobj[e] = 0
	played[e] = 0
end

function crafting_table_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < crafting_table[e].use_range then
			--pinpoint select object--			
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,crafting_table[e].use_range
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
		end
		if PlayerDist < crafting_table[e].use_range and tEnt[e] ~= 0 then
			PromptDuration(crafting_table[e].use_prompt ,1000)	
			if g_KeyPressE == 1 then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_UserGlobalContainer = crafting_table[e].craft_container
				ScreenToggle(crafting_table[e].craft_screen)
			end
		end
	else
		-- in craft screen
	end	
end
