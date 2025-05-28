-- Valve v6 by Necrym59 
-- DESCRIPTION: The attached object will give the player a valve wheel resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Valve Wheel collected"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: <Sound0> when picked up.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_valve = {}

local valve = {}
local prompt_text = {}
local pickup_range = {}
local pickup_style = {}
local collected_text = {}
local prompt_display = {}
local item_highlight = {}
local doonce = {}
local selectobj = {}
local tEnt = {}

function valve_properties(e, prompt_text, pickup_range, pickup_style, collected_text, prompt_display, item_highlight)
	valve[e].prompt_text = prompt_text
	valve[e].pickup_range = pickup_range
	valve[e].pickup_style = pickup_style
	valve[e].collected_text = collected_text	
	valve[e].prompt_display = prompt_display
	valve[e].item_highlight = item_highlight
end

function valve_init_name(e)	
	valve[e] = {}	
	valve[e].prompt_text = "E to collect"	
	valve[e].pickup_range = 80
	valve[e].pickup_style = 2
	valve[e].collected_text = "Valve Wheel collected"	
	valve[e].prompt_display = 1
	valve[e].item_highlight = 0
	g_valve = 0
	doonce[e] = 0
	selectobj[e] = 0
	tEnt[e] = 0
end

function valve_main(e)

	PlayerDist = GetPlayerDistance(e)
	if valve[e].pickup_style == 1 then
		if PlayerDist < valve[e].pickup_range then
			if doonce[e] == 0 then
				if GetEntityCollectable(e) == 0 then
					g_valve = e	
					Hide(e)
					CollisionOff(e)
					Prompt(valve[e].collected_text)
					doonce[e] = 1
				end
				if GetEntityCollectable(e) == 1 or GetEntityCollectable(e) == 2 then -- if collectable or resource
					g_valve = e
					Hide(e)
					CollisionOff(e)
					SetEntityCollected(e,1)
					Prompt(valve[e].collected_text)
					doonce[e] = 1
				end
			end		
		end
	end
	if valve[e].pickup_style == 2 and PlayerDist <= valve[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,valve[e].pickup_range,valve[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--		
		if PlayerDist <= valve[e].pickup_range and tEnt[e] ~= 0 then
			if valve[e].prompt_display == 1 then PromptLocal(e,valve[e].prompt_text) end
			if valve[e].prompt_display == 2 then Prompt(valve[e].prompt_text) end
			if doonce[e] == 0 then
				if GetEntityCollectable(tEnt[e]) == 0 then
					if g_KeyPressE == 1 then
						Destroy(e)
						Prompt(valve[e].collected_text)
						PlaySound(e,0)
						PerformLogicConnections(e)
						g_valve = e
						doonce[e] = 1						
					end
				end
				if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then  -- if collectable or resource
					if g_KeyPressE == 1 then
						Hide(e)
						CollisionOff(e)
						SetEntityCollected(tEnt[e],1)
						Prompt(valve[e].collected_text)
						PlaySound(e,0)
						PerformLogicConnections(e)
						g_valve = e
						doonce[e] = 1	
					end
				end
			end	
		end
	end	
end