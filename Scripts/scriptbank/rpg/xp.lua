-- XP v18
-- DESCRIPTION: The object will give the player an xp addition or deduction if used.
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USAGE_TEXT$="XP consumed"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] User global to be affected (eg: MyXP)
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for usage sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local xp 					= {}
local prompt_text 			= {}
local prompt_if_collectable	= {}
local usage_text			= {}
local quantity				= {}
local pickup_range			= {}
local pickup_style			= {}
local effect				= {}
local user_global_affected	= {}
local prompt_display		= {}
local item_highlight		= {}
local highlight_icon		= {}

local use_item_now	= {}
local status		= {}
local hl_icon		= {}
local hl_imgwidth	= {}
local hl_imgheight	= {}
local tEnt			= {}
local selectobj		= {}
local currentvalue	= {}

function xp_properties(e, prompt_text, prompt_if_collectable, usage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	xp[e].prompt_text = prompt_text
	xp[e].prompt_if_collectable = prompt_if_collectable
	xp[e].usage_text = usage_text
	xp[e].quantity = quantity
	xp[e].pickup_range = pickup_range
	xp[e].pickup_style = pickup_style
	xp[e].effect = effect
	xp[e].user_global_affected = user_global_affected
	xp[e].prompt_display = prompt_display
	xp[e].item_highlight = item_highlight
	xp[e].highlight_icon = highlight_icon_imagefile
end

function xp_init(e)
	xp[e] = {}
	xp[e].prompt_text = "E to consume"
	xp[e].prompt_if_collectable = "E to collect"
	xp[e].usage_text = "XP consumed"
	xp[e].quantity = 10
	xp[e].pickup_range = 80
	xp[e].pickup_style = 1
	xp[e].effect = 1
	xp[e].user_global_affected = ""
	xp[e].prompt_display = 1
	xp[e].item_highlight = 0
	xp[e].highlight_icon = "imagebank\\icons\\pickup.png"

	status[e] = "init"
	use_item_now[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
	selectobj[e] = 0
	currentvalue[e] = 0
end

function xp_main(e)

	if status[e] == "init" then
		if xp[e].item_highlight == 3 and xp[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(xp[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(xp[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(xp[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if xp[e].pickup_style == 1 then
		if PlayerDist < xp[e].pickup_range then
			Prompt(xp[e].usage_text)
			use_item_now[e] = 1
		end
	end
	if xp[e].pickup_style == 2 and PlayerDist < xp[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,xp[e].pickup_range,xp[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < xp[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if xp[e].prompt_display == 1 then PromptLocal(e,xp[e].prompt_text) end
				if xp[e].prompt_display == 2 then Prompt(xp[e].prompt_text) end
				if g_KeyPressE == 1 then
					if xp[e].prompt_display == 1 then PromptLocal(e,xp[e].usage_text) end
					if xp[e].prompt_display == 2 then Prompt(xp[e].usage_text) end
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if xp[e].prompt_display == 1 then PromptLocal(e,xp[e].prompt_if_collectable) end
				if xp[e].prompt_display == 2 then Prompt(xp[e].prompt_if_collectable) end
				-- if collectable or resource
				if g_KeyPressE == 1 then
					Hide(e)
					CollisionOff(e)
					SetEntityCollected(tEnt[e],1)
					PlaySound(e,1)
				end
			end
		end
	end
	local tusedvalue = GetEntityUsed(e)
	if tusedvalue > 0 then
		-- if this is a resource, it will deplete qty and set used to zero
		PromptDuration(xp[e].usage_text,2000)
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if xp[e].effect == 1 then addquantity = 1 end
		if xp[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end
	if addquantity == 1 then
		if xp[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..xp[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..xp[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..xp[e].user_global_affected.."']"] = currentvalue[e] + xp[e].quantity
		end
	end
	if addquantity == 2 then
		if xp[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..xp[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..xp[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..xp[e].user_global_affected.."']"] = currentvalue[e] - xp[e].quantity
		end
	end
end
