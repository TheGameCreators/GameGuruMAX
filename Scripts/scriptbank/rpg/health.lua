-- Health v23 by Necrym59 and Lee
-- DESCRIPTION: The object will give the player an health boost or deduction if used. Can be used as a resource  and a user global can also receive a value.
-- DESCRIPTION: [PROMPT_TEXT$="E to consume"]
-- DESCRIPTION: [PROMPT_IF_COLLECTABLE$="E to collect"]
-- DESCRIPTION: [USEAGE_TEXT$="Health applied"]
-- DESCRIPTION: [QUANTITY=10(1,100)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Ranged, 2=Accurate)]
-- DESCRIPTION: [@EFFECT=1(1=Add, 2=Deduct)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] for use with another User Global g: MyGlobal
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\pickup.png"]
-- DESCRIPTION: <Sound0> for use sound.
-- DESCRIPTION: <Sound1> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local health = {}
local prompt_text = {}
local prompt_if_collectable = {}
local useage_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local pickup_range = {}
local effect = {}
local user_global_affected = {}
local prompt_display = {}
local item_highlight = {}
local highlight_icon = {}

local currentvalue = {}
local addquantity = {}
local selectobj = {}
local tEnt = {}
local status = {}
local hl_icon = {}
local hl_imgwidth = {}
local hl_imgheight = {}

function health_properties(e, prompt_text, prompt_if_collectable, useage_text, quantity, pickup_range, pickup_style, effect, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	health[e].prompt_text = prompt_text
	health[e].prompt_if_collectable = prompt_if_collectable
	health[e].useage_text = useage_text
	health[e].quantity = quantity
	health[e].pickup_range = pickup_range
	health[e].pickup_style = pickup_style
	health[e].effect = effect
	if user_global_affected == nil then user_global_affected = "" end
	health[e].user_global_affected = user_global_affected
	health[e].prompt_display = prompt_display
	health[e].item_highlight = item_highlight
	health[e].highlight_icon = highlight_icon_imagefile	
end

function health_init(e)
	health[e] = {}
	health[e].prompt_text = "E to Use"
	health[e].prompt_if_collectable = "E to collect"
	health[e].useage_text = "Health applied"
	health[e].quantity = 10
	health[e].pickup_range = 80
	health[e].pickup_style = 1
	health[e].effect = 1
	health[e].user_global_affected = ""
	health[e].prompt_display = 1
	health[e].item_highlight = 0
	health[e].highlight_icon = "imagebank\\icons\\pickup.png"
	
	status[e] = "init"
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	currentvalue[e] = 0
	addquantity[e] = 0
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0	
end

function health_main(e)
	
	if status[e] == "init" then
		if health[e].item_highlight == 3 and health[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(health[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(health[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(health[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end
		status[e] = "endinit"
	end	

	local use_item_now = 0
	local PlayerDist = GetPlayerDistance(e)

	if health[e].pickup_style == 1 then
		if PlayerDist < health[e].pickup_range then
			if health[e].prompt_display == 1 then PromptLocal(e,health[e].useage_text) end
			if health[e].prompt_display == 2 then Prompt(health[e].useage_text) end
			use_item_now = 1
		end
	end

	if health[e].pickup_style == 2 and PlayerDist < health[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,health[e].pickup_range,health[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--	

		if PlayerDist < health[e].pickup_range and tEnt[e] == e then
			if GetEntityCollectable(tEnt[e]) == 0 then
				if health[e].prompt_display == 1 then PromptLocal(e,health[e].prompt_text) end
				if health[e].prompt_display == 2 then Prompt(health[e].prompt_text) end
				if g_KeyPressE == 1 then
					use_item_now = 1
				end
			end
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				-- if collectable or resource
				if health[e].prompt_display == 1 then PromptLocal(e,health[e].prompt_if_collectable) end
				if health[e].prompt_display == 2 then Prompt(health[e].prompt_if_collectable) end				
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
		SetEntityUsed(e,tusedvalue*-1)
		if health[e].prompt_display == 1 then PromptLocal(e,health[e].useage_text) end
		if health[e].prompt_display == 2 then Prompt(health[e].useage_text) end		
		use_item_now = 1
	end

	if use_item_now == 1 then
		use_item_now = 0
		PlayNon3DSound(e,0)
		PerformLogicConnections(e)
		if health[e].effect == 1 then addquantity[e] = 1 end
		if health[e].effect == 2 then addquantity[e] = 2 end
		Destroy(e) -- can only destroy resources that are qty zero
	end

	if addquantity[e] == 1 then
		SetPlayerHealth(g_PlayerHealth + health[e].quantity)
		if g_PlayerHealth > g_PlayerStartStrength then g_PlayerHealth = g_PlayerStartStrength end
		SetPlayerHealthCore(g_PlayerHealth)
		if health[e].user_global_affected ~= "" then
			if _G["g_UserGlobal['"..health[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..health[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..health[e].user_global_affected.."']"] = currentvalue[e] + health[e].quantity
		end
	end
	if addquantity[e] == 2 then
		SetPlayerHealth(g_PlayerHealth - health[e].quantity)
		SetPlayerHealthCore(g_PlayerHealth)
		if health[e].user_global_affected ~= "" then
			if _G["g_UserGlobal['"..health[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..health[e].user_global_affected.."']"] end
			_G["g_UserGlobal['"..health[e].user_global_affected.."']"] = currentvalue[e] - health[e].quantity
		end
	end
end
