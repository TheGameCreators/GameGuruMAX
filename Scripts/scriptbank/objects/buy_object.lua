-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Buy Object v7 Necrym59 and oosayeroo 
-- DESCRIPTION: The object will act as a switch to trigger logic connection if the global affected variable amount is valid.
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [BUY_TEXT$="E to purchase"]
-- DESCRIPTION: [BOUGHT_TEXT$="Purchased"]
-- DESCRIPTION: [INSUFFICIENT_TEXT$="Insufficient funds"]
-- DESCRIPTION: [@TEXT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [BUY_COST=10(1,500)]
-- DESCRIPTION: [@DELETE_ON_BUY=1(1=Yes, 2=No)]
-- DESCRIPTION: [@@USER_GLOBAL_AFFECTED$=""(0=globallist)] eg: MyMoney
-- DESCRIPTION: <Sound0> for buying sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local buyobject 			= {}
local use_range 			= {}
local buy_text 				= {}
local bought_text 			= {}
local insufficient_text 	= {}
local text_display			= {}
local buy_cost 				= {}
local delete_on_buy 		= {}
local user_global_affected	= {}

local tEnt 				= {}
local selectobj 		= {}
local played 			= {}
local currentvalue 		= {}
local status      		= {}

function buy_object_properties(e, use_range, buy_text, bought_text, insufficient_text, text_display, buy_cost, delete_on_buy, user_global_affected)
	buyobject[e].use_range = use_range	
	buyobject[e].buy_text = buy_text
	buyobject[e].bought_text = bought_text
	buyobject[e].insufficient_text = insufficient_text
	buyobject[e].text_display = text_display	
	buyobject[e].buy_cost = buy_cost
	buyobject[e].delete_on_buy = delete_on_buy
	buyobject[e].user_global_affected = user_global_affected
end

function buy_object_init(e)
	buyobject[e] = {}
	buyobject[e].use_range = 80	
	buyobject[e].buy_text = "E to purchase"
	buyobject[e].bought_text = "Purchased"
	buyobject[e].insufficient_text = "Insufficient funds"
	buyobject[e].text_display = 1	
	buyobject[e].buy_cost = 1
	buyobject[e].delete_on_buy = 1
	buyobject[e].user_global_affected = ""	
	
	currentvalue[e] = 0
	played[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	status[e] = "init"
end

function buy_object_main(e)
	if status[e] == "init" then
		status[e] = "ready"
	end
	
	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < buyobject[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,buyobject[e].use_range,0)
		tEnt[e] = g_tEnt
		--end pinpoint select object--	
	end		
	if PlayerDist < buyobject[e].use_range and tEnt[e] ~= 0 then
		if status[e] == "ready" then
			if buyobject[e].text_display == 1 then PromptLocal(e,buyobject[e].buy_text) end
			if buyobject[e].text_display == 2 then Prompt(buyobject[e].buy_text) end
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
						if buyobject[e].text_display == 1 then PromptLocal(e,buyobject[e].insufficient_text) end
						if buyobject[e].text_display == 2 then PromptDuration(buyobject[e].insufficient_text,2000) end					
					end
				end	
			end	
		end
	end
	
	if status[e] == "bought" then
		if buyobject[e].text_display == 1 then PromptLocal(e,buyobject[e].bought_text) end
		if buyobject[e].text_display == 2 then PromptDuration(buyobject[e].bought_text,2000) end
		if buyobject[e].delete_on_buy == 1 then
			CollisionOff(e)
			Destroy(e)
		end
        status[e] = "end"
		SwitchScript(e,"no_behavior_selected.lua")
    end
end
