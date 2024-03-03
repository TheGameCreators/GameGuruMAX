-- Shop v4
-- DESCRIPTION: The attached object can be used as a shop.
-- DESCRIPTION: When player is within [USE_RANGE=100],
-- DESCRIPTION: show [USE_PROMPT$="Press E to shop"] and
-- DESCRIPTION: when use key is pressed, will open the [SHOP_SCREEN$="HUD Screen 5"]
-- DESCRIPTION: using [SHOP_CONTAINER$="shop"]
-- DESCRIPTION: The shop is called [SHOP_NAME$="The Shop"]
-- DESCRIPTION: <Sound0> when crafting started.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

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
	g_tEnt = 0
	selectobj[e] = 0
	played[e] = 0
end

function shop_main(e)
	if GetCurrentScreen() == -1 then
		-- in the game
		local PlayerDist = GetPlayerDistance(e)
		if PlayerDist < shop[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,shop[e].use_range,300)
			tEnt[e] = g_tEnt
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
