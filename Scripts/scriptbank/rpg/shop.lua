-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Shop v8 by Necrym59
-- DESCRIPTION: The attached object can be used as a shop to trade goods.
-- DESCRIPTION: When player is within [USE_RANGE=100],
-- DESCRIPTION: show [USE_PROMPT$="Press E to shop"] and
-- DESCRIPTION: when use key is pressed, will open the [@@SHOP_SCREEN$="HUD Screen 5"(0=hudscreenlist)] Eg: HUD Screen 5
-- DESCRIPTION: using [SHOP_CONTAINER$=""] eg"shop"
-- DESCRIPTION: The shop is called [SHOP_NAME$="The Shop"]
-- DESCRIPTION: [LEVEL_BIAS!=0] Shop container renamed based on player level. For example "shop" container would become "shop5" if level 5 player.
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\trade.png"]
-- DESCRIPTION: <Sound0> when starting to use.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local shop 				= {}
local use_range 		= {}
local use_prompt 		= {}
local shop_screen 		= {}
local shop_container 	= {}
local shop_name 		= {}
local level_bias		= {}
local prompt_display 	= {}
local item_highlight 	= {}
local highlight_icon 	= {}

local tEnt 				= {}
local selectobj			= {}
local played			= {}
local status 			= {}
local hl_icon 			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

function shop_properties(e, use_range, use_prompt, shop_screen, shop_container, shop_name, level_bias, prompt_display, item_highlight, highlight_icon_imagefile)
	shop[e].use_range = use_range
	shop[e].use_prompt = use_prompt
	shop[e].shop_screen = shop_screen
	shop[e].shop_container = shop_container
	shop[e].shop_name = shop_name
	shop[e].level_bias = level_bias or 0
	shop[e].prompt_display = prompt_display
	shop[e].item_highlight = item_highlight
	shop[e].highlight_icon = highlight_icon_imagefile
end

function shop_init(e)
	shop[e] = {}
	shop[e].use_range = 100
	shop[e].use_prompt = "Press E to shop"
	shop[e].shop_screen = "HUD Screen 5"
	shop[e].shop_container = "shop"
	shop[e].shop_name = "The Shop"
	shop[e].level_bias = 0
	shop[e].prompt_display = 1
	shop[e].item_highlight = 0
	shop[e].highlight_icon = "imagebank\\icons\\trade.png"

	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	played[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function shop_main(e)

	if status[e] == "init" then
		if shop[e].item_highlight == 3 and shop[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(shop[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(shop[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(shop[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	if GetCurrentScreen() == -1 then
		-- in the game
		local PlayerDist = GetPlayerDistance(e)

		if PlayerDist < shop[e].use_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,shop[e].use_range,shop[e].item_highlight,hl_icon[e])
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end

		if PlayerDist < shop[e].use_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if shop[e].prompt_display == 1 then PromptLocal(e,shop[e].use_prompt) end
			if shop[e].prompt_display == 2 then Prompt(shop[e].use_prompt) end
			if g_KeyPressE == 1 then
				if played[e] == 0 then
					PlaySound(e,0)
					played[e] = 1
				end
				if shop[e].level_bias == 0 then g_UserGlobalContainer = shop[e].shop_container end
				if shop[e].level_bias == 1 then g_UserGlobalContainer = shop[e].shop_container.. "" .._G["g_UserGlobal['".."MyPlayerLevel".."']"] end
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