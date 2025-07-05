-- DESCRIPTION: The object will give the player a stamina boost or deduction if used.
-- Stamina v16
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USAGE_TEXT$="Stamina consumed"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyStaminaMax
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for usage sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local stamina = {}
local prompt_text = {}
local prompt_if_collectable = {}
local usage_text, quantity = {}
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

function stamina_properties(e, prompt_text, prompt_if_collectable, usage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	stamina[e].prompt_text = prompt_text
	stamina[e].prompt_if_collectable = prompt_if_collectable
	stamina[e].usage_text = usage_text
	stamina[e].quantity = quantity
	stamina[e].pickup_range = pickup_range
	stamina[e].pickup_style = pickup_style
	stamina[e].effect = effect
	stamina[e].user_global_affected = user_global_affected
	stamina[e].prompt_display = prompt_display
	stamina[e].item_highlight = item_highlight
	stamina[e].highlight_icon = highlight_icon_imagefile	
end

function stamina_init(e)
	stamina[e] = {}
	stamina[e].prompt_text = "E to consume"
	stamina[e].prompt_if_collectable = "E to collect"
	stamina[e].usage_text = "Stamina consumed"
	stamina[e].quantity = 10
	stamina[e].pickup_range = 80
	stamina[e].pickup_style = 1
	stamina[e].effect = 1
	stamina[e].user_global_affected = "MyStaminaMax"
	stamina[e].prompt_display = 1
	stamina[e].item_highlight = 0
	stamina[e].highlight_icon = "imagebank\\icons\\pickup.png"	

	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
	use_item_now[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0	
end

function stamina_main(e)

	if status[e] == "init" then
		if stamina[e].item_highlight == 3 and stamina[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(stamina[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(stamina[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(stamina[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if stamina[e].pickup_style == 1 then
		if PlayerDist < stamina[e].pickup_range then
			PromptDuration(stamina[e].usage_text,2000)
			use_item_now[e] = 1
		end
	end
	if stamina[e].pickup_style == 2 and PlayerDist < stamina[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,stamina[e].pickup_range,stamina[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < stamina[e].pickup_range and tEnt[e] == e and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if stamina[e].prompt_display == 1 then PromptLocal(e,stamina[e].prompt_text) end
				if stamina[e].prompt_display == 2 then Prompt(stamina[e].prompt_text) end
				if g_KeyPressE == 1 then
					PromptDuration(stamina[e].usage_text,2000)
					use_item_now[e] = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if stamina[e].prompt_display == 1 then PromptLocal(e,stamina[e].prompt_if_collectable) end
				if stamina[e].prompt_display == 2 then Prompt(stamina[e].prompt_if_collectable) end
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
		PromptDuration(stamina[e].usage_text,2000)
		SetEntityUsed(e,tusedvalue*-1)
		use_item_now[e] = 1
	end
	local addquantity = 0
	if use_item_now[e] == 1 then
		PlaySound(e,0)
		PerformLogicConnections(e)
		if stamina[e].effect == 1 then addquantity = 1 end
		if stamina[e].effect == 2 then addquantity = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end
	local currentvalue = 0
	if addquantity == 1 then
		if stamina[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..stamina[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..stamina[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..stamina[e].user_global_affected.."']"] = currentvalue + stamina[e].quantity
		end
	end
	if addquantity == 2 then
		if stamina[e].user_global_affected > "" then
			if _G["g_UserGlobal['"..stamina[e].user_global_affected.."']"] ~= nil then currentvalue = _G["g_UserGlobal['"..stamina[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..stamina[e].user_global_affected.."']"] = currentvalue - stamina[e].quantity
		end
	end
end
