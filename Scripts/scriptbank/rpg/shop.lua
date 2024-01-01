-- Shop v3
-- DESCRIPTION: The attached object can be used as a shop.
-- DESCRIPTION: When player is within [USE_RANGE=100],
-- DESCRIPTION: show [USE_PROMPT$="Press E to shop"] and
-- DESCRIPTION: when use key is pressed, will open the [SHOP_SCREEN$="HUD Screen 5"]
-- DESCRIPTION: using [SHOP_CONTAINER$="shop"]
-- DESCRIPTION: The shop is called [SHOP_NAME$="The Shop"]
-- DESCRIPTION: <Sound0> when crafting started.

local U = require "scriptbank\\utillib"
local shop 				= {}
local use_range 		= {}
local use_prompt 		= {}
local shop_screen 		= {}
local shop_container 	= {}
local shop_name 		= {}
local tEnt 				= {}
local selectobj			= {}
local played			= {}

function shop_properties(e, use_range, use_prompt, shop_screen, shop_container, shop_name)
	shop[e].use_range = use_range
	shop[e].use_prompt = use_prompt
	shop[e].shop_screen = shop_screen
	shop[e].shop_container = shop_container
	shop[e].shop_name = shop_name
end

function shop_init(e)
	shop[e] = {}
	shop[e].use_range = 100
	shop[e].use_prompt = "Press E to shop"
	shop[e].shop_screen = "HUD Screen 5"
	shop[e].shop_container = "shop"
	shop[e].shop_name = "The Shop"
	tEnt[e] = 0
	selectobj[e] = 0
	played[e] = 0
end

function shop_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < shop[e].use_range then
			--pinpoint select object--			
			local px, py, pz = GetCameraPositionX(0), GetCameraPositionY(0), GetCameraPositionZ(0)
			local rayX, rayY, rayZ = 0,0,shop[e].use_range
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
		if PlayerDist < shop[e].use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			PromptDuration(shop[e].use_prompt ,1000)	
			if g_KeyPressE == 1 then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				g_UserGlobalContainer = shop[e].shop_container
				local tuserglobal = "MyShopName"
				local tuservalue = shop[e].shop_name
				 _G["g_UserGlobal['"..tuserglobal.."']"] = tuservalue
				ScreenToggle(shop[e].shop_screen)
			end
		end
	else
		-- in shop screen
	end	
end
