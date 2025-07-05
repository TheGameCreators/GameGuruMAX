-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Buy Object v8 Necrym59
-- DESCRIPTION: The object will act as a switch to trigger logic connection if the global affected variable amount is valid.
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [BUY_TEXT$="E to purchase"]
-- DESCRIPTION: [BOUGHT_TEXT$="Purchased"]
-- DESCRIPTION: [INSUFFICIENT_TEXT$="Insufficient funds"]
-- DESCRIPTION: [BUY_COST=10(1,500)]
-- DESCRIPTION: [@DELETE_ON_BUY=1(1=Yes, 2=No)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyMoney
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline,3=Icon)]
-- DESCRIPTION: [HIGHLIGHT_ICON_IMAGEFILE$="imagebank\\icons\\trade.png"]
-- DESCRIPTION: <Sound0> for buying sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local buyobject 			= {}
local use_range 			= {}
local buy_text 				= {}
local bought_text 			= {}
local insufficient_text 	= {}
local buy_cost 				= {}
local delete_on_buy 		= {}
local user_global_affected	= {}
local prompt_display		= {}
local item_highlight		= {}
local highlight_icon		= {}

local tEnt 				= {}
local selectobj 		= {}
local played 			= {}
local currentvalue 		= {}
local status      		= {}
local hl_icon			= {}
local hl_imgwidth		= {}
local hl_imgheight		= {}

function buy_object_properties(e, use_range, buy_text, bought_text, insufficient_text, buy_cost, delete_on_buy, user_global_affected, prompt_display, item_highlight, highlight_icon_imagefile)
	buyobject[e].use_range = use_range	
	buyobject[e].buy_text = buy_text
	buyobject[e].bought_text = bought_text
	buyobject[e].insufficient_text = insufficient_text
	buyobject[e].buy_cost = buy_cost
	buyobject[e].delete_on_buy = delete_on_buy
	buyobject[e].user_global_affected = user_global_affected
	buyobject[e].prompt_display = prompt_display
	buyobject[e].item_highlight = item_highlight
	buyobject[e].highlight_icon = highlight_icon_imagefile	
end

function buy_object_init(e)
	buyobject[e] = {}
	buyobject[e].use_range = 80	
	buyobject[e].buy_text = "E to purchase"
	buyobject[e].bought_text = "Purchased"
	buyobject[e].insufficient_text = "Insufficient funds"
	buyobject[e].buy_cost = 1
	buyobject[e].delete_on_buy = 1
	buyobject[e].user_global_affected = ""
	buyobject[e].prompt_display = 1
	buyobject[e].item_highlight = 0
	buyobject[e].highlight_icon = "imagebank\\icons\\trade.png"
	
	currentvalue[e] = 0
	played[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	status[e] = "init"
	hl_icon[e] = 0
	hl_imgwidth[e] = 0
	hl_imgheight[e] = 0
end

function buy_object_main(e)
	if status[e] == "init" then
		if buyobject[e].item_highlight == 3 and buyobject[e].highlight_icon ~= "" then
			hl_icon[e] = CreateSprite(LoadImage(buyobject[e].highlight_icon))
			hl_imgwidth[e] = GetImageWidth(LoadImage(buyobject[e].highlight_icon))
			hl_imgheight[e] = GetImageHeight(LoadImage(buyobject[e].highlight_icon))
			SetSpriteSize(hl_icon[e],-1,-1)
			SetSpriteDepth(hl_icon[e],100)
			SetSpriteOffset(hl_icon[e],hl_imgwidth[e]/2.0, hl_imgheight[e]/2.0)
			SetSpritePosition(hl_icon[e],500,500)
		end	
		status[e] = "ready"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < buyobject[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,buyobject[e].use_range,buyobject[e].item_highlight,hl_icon[e])
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
	end		
	if PlayerDist < buyobject[e].use_range and tEnt[e] == e then
		if status[e] == "ready" then
			if buyobject[e].prompt_display == 1 then PromptLocal(e,buyobject[e].buy_text) end
			if buyobject[e].prompt_display == 2 then Prompt(buyobject[e].buy_text) end
			if g_KeyPressE == 1 then
				if buyobject[e].user_global_affected > "" then
					if _G["g_UserGlobal['"..buyobject[e].user_global_affected.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..buyobject[e].user_global_affected.."']"] end
					if currentvalue[e] >= buyobject[e].buy_cost then
						_G["g_UserGlobal['"..buyobject[e].user_global_affected.."']"] = currentvalue[e] - buyobject[e].buy_cost
						PerformLogicConnections(e)
						if played[e] == 0 then
							PlaySound(e,0)
							played[e] = 1
						end
						status[e] = "bought"
					else
						if buyobject[e].prompt_display == 1 then PromptLocal(e,buyobject[e].insufficient_text) end
						if buyobject[e].prompt_display == 2 then PromptDuration(buyobject[e].insufficient_text,2000) end					
					end
				end	
			end	
		end
	end
	
	if status[e] == "bought" then
		if buyobject[e].prompt_display == 1 then PromptLocal(e,buyobject[e].bought_text) end
		if buyobject[e].prompt_display == 2 then PromptDuration(buyobject[e].bought_text,2000) end
		if buyobject[e].delete_on_buy == 1 then
			CollisionOff(e)
			Destroy(e)
		end
        status[e] = "end"
		SwitchScript(e,"no_behavior_selected.lua")
    end
end
