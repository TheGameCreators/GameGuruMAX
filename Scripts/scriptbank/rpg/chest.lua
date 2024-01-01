-- Chest v4
-- DESCRIPTION: When player is within [USE_RANGE=100], show
-- DESCRIPTION: [USE_PROMPT$="Press E to open"] when use key is pressed,
-- DESCRIPTION: will display [CHEST_SCREEN$="HUD Screen 6"],
-- DESCRIPTION: using [CHEST_CONTAINER$="chestunique"].
-- DESCRIPTION: <Sound0> for opening chest.
-- DESCRIPTION: <Sound1> for closing chest.

local U = require "scriptbank\\utillib"
local chest = {}
local use_range = {}
local use_prompt = {}
local chest_screen = {}
local chest_container = {}
local status = {}
local doonce = {}
local tEnt = {}
local selectobj = {}

function chest_properties(e, use_range, use_prompt, chest_screen, chest_container)
	chest[e].use_range = use_range
	chest[e].use_prompt = use_prompt
	chest[e].chest_screen = chest_screen
	chest[e].chest_container = chest_container
end

function chest_init(e)
	chest[e] = {}
	chest[e].use_range = 100
	chest[e].use_prompt = "Press E to open"
	chest[e].chest_screen = "HUD Screen 6"
	chest[e].chest_container = "chestunique"
	status[e] = "closed"
	selectobj[e] = 0
	tEnt[e] = 0
end

function chest_main(e)
	if GetCurrentScreen() == -1 and status[e] == "closed" then
		-- in the game
		PlayerDist = GetPlayerDistance(e)
		if PlayerDist < chest[e].use_range then
			--pinpoint select object--
			TextColor(50,50,3,"+",100,100,100) --(pointer crosshair at present)
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,chest[e].use_range
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
		if PlayerDist < chest[e].use_range and tEnt[e] ~= 0 then
			PromptDuration(chest[e].use_prompt ,1000)
			if g_KeyPressE == 1 then
				SetAnimationName(e,"open")
				PlayAnimation(e)
				PlaySound(e,0)
				if chest[e].chest_container == "chestunique" then
					g_UserGlobalContainer = "chestuniquetolevelase"..tostring(e)
				else
					g_UserGlobalContainer = chest[e].chest_container
				end
				ScreenToggle(chest[e].chest_screen)
				status[e] = "opened"				
			end
		end
	else
		-- in chest screen
	end
	if GetCurrentScreen() == -1 and status[e] == "opened" then
		SetAnimationName(e,"close")
		PlayAnimation(e)
		PlaySound(e,1)
		status[e] = "closed"
	end		
end
