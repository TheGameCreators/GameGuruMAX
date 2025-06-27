-- DESCRIPTION: The object will give the player a health boost if collected.
-- Oxygen v6 - by Necrym59
-- DESCRIPTION: The object will give the player a oxygen boost and health boost if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [HEALTH_QUANTITY=10(1,40)]
-- DESCRIPTION: [OXYGEN_QUANTITY=10(1,80)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [COLLECTED_TEXT$="Collected oxygen supply"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: Play <Sound0> when the object is picked up by the player.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local oxygen = {}
local prompt_text = {}
local heath_quantity = {}
local oxygen_quantity = {}
local pickup_range = {}
local pickup_style = {}
local collected_text = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local selectobj = {}
local tEnt = {}
local currentvalue = {}	

function oxygen_properties(e, prompt_text, heath_quantity, oxygen_quantity, pickup_range, pickup_style, collected_text, prompt_display, item_highlight, highlight_icon_imagefile)
	oxygen[e].prompt_text = prompt_text
	oxygen[e].heath_quantity = heath_quantity
	oxygen[e].oxygen_quantity = oxygen_quantity
	oxygen[e].pickup_range = pickup_range
	oxygen[e].pickup_style = pickup_style
	oxygen[e].collected_text = collected_text
	oxygen[e].prompt_display = prompt_display
	oxygen[e].item_highlight = item_highlight
	oxygen[e].highlight_icon = highlight_icon_imagefile
	if user_global_affected == nil then user_global_affected = "" end
end

function oxygen_init(e)
	oxygen[e] = {}
	oxygen[e].prompt_text = "E to use"
	oxygen[e].heath_quantity = 10
	oxygen[e].oxygen_quantity = 40
	oxygen[e].pickup_range = 80
	oxygen[e].pickup_style = 1
	oxygen[e].collected_text = "Collected oxygen supply"
	oxygen[e].prompt_display = 1
	oxygen[e].item_highlight = 0
	oxygen[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	currentvalue[e] = 0
end

function oxygen_main(e)

	if status[e] == "init" then
		if oxygen[e].item_highlight == 3 and oxygen[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(oxygen[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(oxygen[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(oxygen[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	PlayerDist = GetPlayerDistance(e)
	if oxygen[e].pickup_style == 1 then
		if PlayerDist < oxygen[e].pickup_range and g_PlayerHealth > 0 then
			PromptDuration(oxygen[e].collected_text,2000)
			PlaySound(e,0)
			PerformLogicConnections(e)
			ActivateIfUsed(e)
			SetPlayerHealth(g_PlayerHealth + oxygen[e].heath_quantity)
			local alreadyhaveair = GetGamePlayerControlDrownTimestamp()-Timer()
			SetGamePlayerControlDrownTimestamp(Timer()+(oxygen[e].oxygen_quantity*100)+alreadyhaveair)
			Destroy(e)
		end
	end
	if oxygen[e].pickup_style == 2 and PlayerDist < oxygen[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,oxygen[e].pickup_range,oxygen[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		if PlayerDist < oxygen[e].pickup_range and tEnt[e] == e then
			if oxygen[e].prompt_display == 1 then PromptLocal(e,oxygen[e].prompt_text) end
			if oxygen[e].prompt_display == 2 then Prompt(oxygen[e].prompt_text) end
			if g_KeyPressE == 1 then
				PromptDuration(oxygen[e].collected_text,2000)
				PlaySound(e,0)
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				SetPlayerHealth(g_PlayerHealth + oxygen[e].heath_quantity)
				local alreadyhaveair = GetGamePlayerControlDrownTimestamp()-Timer()
				SetGamePlayerControlDrownTimestamp(Timer()+(oxygen[e].oxygen_quantity*100)+alreadyhaveair)
				Destroy(e)
			end
		end
	end
end

