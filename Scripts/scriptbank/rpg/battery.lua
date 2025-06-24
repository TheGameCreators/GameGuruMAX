-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Battery v10 by Necrym59
-- DESCRIPTION: The attached object will give the player a battery energy resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Battery collected"]
-- DESCRIPTION: [ENERGY_LEVEL=10(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=90(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] Eg: MyBatteryEnergy
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_batteryenergy = {}

local battery 				= {}
local prompt_text 			= {}
local collected_text 		= {}
local energy_level 			= {}
local pickup_range 			= {}
local pickup_style 			= {}
local user_global_affected	= {}
local prompt_display 		= {}
local item_highlight 		= {}
local highlight_icon 		= {}

local currentvalue 		= {}
local pressed 			= {}
local played 			= {}
local status			= {}
local tEnt				= {}
local selectobj			= {}
local hl_icon 			= {}
local hl_imgwidth 		= {}
local hl_imgheight 		= {}

function battery_properties(e, prompt_text, collected_text, energy_level, pickup_range, pickup_style, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	battery[e].prompt_text = prompt_text
	battery[e].collected_text = collected_text
	battery[e].energy_level = energy_level
	battery[e].pickup_range = pickup_range
	battery[e].pickup_style = pickup_style
	battery[e].user_global_affected = user_global_affected
	battery[e].prompt_display = prompt_display
	battery[e].item_highlight = item_highlight	
	battery[e].highlight_icon = highlight_icon_imagefile
end

function battery_init(e)
	battery[e] = {}
	battery[e].prompt_text = "E to collect"
	battery[e].collected_text = "Collected battery"
	battery[e].energy_level = 0
	battery[e].pickup_range = 80
	battery[e].pickup_style = 1
	battery[e].user_global_affected = "MyBatteryEnergy"
	battery[e].prompt_display = 1
	battery[e].item_highlight = 0	
	battery[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	g_batteryenergy = 0
	played[e] = 0
	pressed[e] = 0
	currentvalue[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	status[e] = "init"
end

function battery_main(e)

	if status[e] == "init" then
		if battery[e].item_highlight == 3 and battery[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(battery[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(battery[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(battery[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)

	if battery[e].pickup_style == 1 and PlayerDist < battery[e].pickup_range then
		if battery[e].prompt_display == 1 then PromptLocal(e,battery[e].collected_text) end
		if battery[e].prompt_display == 2 then Prompt(battery[e].collected_text) end
		if played[e] == 0 then
			PlaySound(e,0)
			played[e] = 1
		end
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
		tEnt[e] = 0
	end

	if battery[e].pickup_style == 2 and PlayerDist < battery[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,battery[e].pickup_range,battery[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
	end

	if PlayerDist < battery[e].pickup_range and tEnt[e] == e then
		if battery[e].prompt_display == 1 then PromptLocal(e,battery[e].prompt_text) end
		if battery[e].prompt_display == 2 then Prompt(battery[e].prompt_text) end
		if g_KeyPressE == 1 and pressed[e] == 0 then
			if battery[e].prompt_display == 1 then PromptLocal(e,battery[e].collected_text) end
			if battery[e].prompt_display == 2 then Prompt(battery[e].collected_text) end
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