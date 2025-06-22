-- DESCRIPTION: The object will give the player an armour boost or deduction if used.
-- Armour v16
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Armour applied"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyArmour
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for use sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local armour = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text, quantity = {}
local pickup_range = {}
local pickup_style = {}
local effect = {}
local user_global_affected = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}
local use_item_now = {}
local tEnt = {}
local selectobj = {}

function armour_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	armour[e].prompt_text = prompt_text
	armour[e].prompt_if_collectable = prompt_if_collectable
	armour[e].useage_text = useage_text
	armour[e].quantity = quantity
	armour[e].pickup_range = pickup_range
	armour[e].pickup_style = pickup_style
	armour[e].effect = effect
	armour[e].user_global_affected = user_global_affected or ""
	armour[e].prompt_display = prompt_display
	armour[e].item_highlight = item_highlight
	armour[e].highlight_icon = highlight_icon_imagefile
end

function armour_init(e)
	armour[e] = {}
	armour[e].prompt_text = "E to Use"
	armour[e].prompt_if_collectable = "E to collect"
	armour[e].useage_text = "Armour worn"
	armour[e].quantity = 10
	armour[e].pickup_range = 80
	armour[e].pickup_style = 1
	armour[e].effect = 1
	armour[e].user_global_affected = "MyArmour"
	armour[e].prompt_display = 1
	armour[e].item_highlight = 0
	armour[e].highlight_icon = "imagebank\\icons\\pickup.png"

	use_item_now[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function armour_main(e)

	if status[e] == "init" then
		if armour[e].item_highlight == 3 and armour[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(armour[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(armour[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(armour[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if armour[e].pickup_style == 1 then
		if PlayerDist < armour[e].pickup_range then
			if armour[e].prompt_display == 1 then PromptLocal(e,armour[e].useage_text) end
			if armour[e].prompt_display == 2 then Prompt(armour[e].useage_text) end
			use_item_now[e] = 1
		end
	end
	if armour[e].pickup_style == 2 and PlayerDist < armour[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,armour[e].pickup_range,armour[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < armour[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if armour[e].prompt_display == 1 then PromptLocal(e,armour[e].prompt_text) end
				if armour[e].prompt_display == 2 then Prompt(armour[e].prompt_text) end
				if g_KeyPressE == 1 then
					use_item_now[e] = 1
					if armour[e].prompt_display == 1 then PromptLocal(e,armour[e].useage_text) end
					if armour[e].prompt_display == 2 then Prompt(armour[e].useage_text) end
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if armour[e].prompt_display == 1 then PromptLocal(e,armour[e].prompt_if_collectable) end
				if armour[e].prompt_display == 2 then Prompt(armour[e].prompt_if_collectable) end
				-- if collectable or resource
				if g_KeyPressE == 1 then
					Hide(e)
					CollisionOff(e)
					SetEntityCollected(tEnt[e],1)
					PlayNon3DSound(e,1)
				end
			end
		end
	end

	local tusedvalue = GetEntityUsed(e)
	if tusedvalue > 0 then
		-- if this is a resource, it will deplete qty and set used to zero
		PromptDuration(armour[e].useage_text,2000)
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlayNon3DSound(e,0)
		PerformLogicConnections(e)
		if armour[e].effect == 1 then addquantity = 1 end
		if armour[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end
	local currentvalue = 0
	if addquantity == 1 then
		if armour[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..armour[e].user_global_affected.."']"] = currentvalue + armour[e].quantity
		end
	end
	if addquantity == 2 then
		if armour[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..armour[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..armour[e].user_global_affected.."']"] = currentvalue - armour[e].quantity
		end
	end
end
