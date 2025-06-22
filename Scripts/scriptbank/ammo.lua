-- Ammo v15 by Necrym and Lee
-- DESCRIPTION: The object will give the player ammunition if collected, and optionally play <Sound0> for collection.
-- DESCRIPTION: You can change the [PROMPTTEXT$="Press E or LMB to collect"]
-- DESCRIPTION: Set the Ammo [&QUANTITY=(1,100)]
-- DESCRIPTION: Set the [PICKUP_RANGE=90(1,200)]
-- DESCRIPTION: and [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: You can change the [COLLECTION_TEXT$="Ammunition collected"]
-- DESCRIPTION: [!PLAY_PICKUP=1]
-- DESCRIPTION: [!ACTIVATE_LOGIC=1]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)] Use emmisive color for shape option
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local g_ammo 			= {}
local status 			= {}
local hl_icon	 		= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

function ammo_properties(e, prompttext, quantity, pickup_range, pickup_style, collection_text, play_pickup, activate_logic, item_highlight, highlight_icon_imagefile)
	g_ammo[e]['prompttext'] = prompttext or "E to use"
	g_ammo[e]['quantity'] = quantity or 0
	g_ammo[e]['pickup_range'] = pickup_range or 90
	g_ammo[e]['pickup_style'] = pickup_style or 1
	g_ammo[e]['collection_text'] = collection_text or "Ammunition collected"
	g_ammo[e]['play_pickup'] = play_pickup or 1
	g_ammo[e]['activate_logic'] = activate_logic or 1
	g_ammo[e]['item_highlight'] = item_highlight or 0
	g_ammo[e]['highlight_icon'] = highlight_icon_imagefile
end

function ammo_init(e)
	g_ammo[e] = {}
	g_ammo[e]['prompttext'] = "E to use"
	g_ammo[e]['quantity'] = 0
	g_ammo[e]['pickup_range'] = 90
	g_ammo[e]['pickup_style'] = 1
	g_ammo[e]['collection_text'] = "Ammunition collected"
	g_ammo[e]['play_pickup'] = 1
	g_ammo[e]['activate_logic'] = 1
	g_ammo[e]['item_highlight'] = 0
	g_ammo[e]['highlight_icon'] = "imagebank\\icons\\pickup.png"
	
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function ammo_main(e)
	if status[e] == "init" then
		if g_ammo[e]['item_highlight'] == 3 and g_ammo[e]['highlight_icon'] ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(g_ammo[e]['highlight_icon']))
			hl_imgwidth[e] = GetImageWidth(LoadImage(g_ammo[e]['highlight_icon']))
			hl_imgheight[e] = GetImageHeight(LoadImage(g_ammo[e]['highlight_icon']))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end	
	PlayerDist = GetPlayerDistance(e)
	local triggerthepickup = 0
	if g_ammo[e]['pickup_style'] == 1 then
		if PlayerDist < g_ammo[e]['pickup_range'] and g_PlayerHealth > 0 and GetEntityVisibility(e) == 1 then
			triggerthepickup = 1
		end
	end
	if g_ammo[e]['pickup_style'] == 2 then
		--pinpoint select object--
		module_misclib.pinpoint(e,g_ammo[e]['pickup_range'],g_ammo[e]['item_highlight'],hl_icon[e])
		--end pinpoint select object--
		if PlayerDist < g_ammo[e]['pickup_range'] and g_tEnt == e then
			PromptLocalForVR(e,g_ammo[e]['prompttext'])
			if g_KeyPressE == 1 or g_MouseClick == 1 then
				SetGamePlayerStateFiringMode(2)
				triggerthepickup = 1
			end
		end
	end
	if triggerthepickup == 1 then
		if g_ammo[e]['play_pickup'] == 1 then
			PlayNon3DSound(e,0)
		end
		if g_ammo[e]['activate_logic'] == 1 then
			PerformLogicConnections(e)
			ActivateIfUsed(e)
		end
		AddPlayerAmmo(e)
		PromptDuration(g_ammo[e]['collection_text'],1000)
		Destroy(e)
	end
end
