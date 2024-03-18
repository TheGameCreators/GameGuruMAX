-- Collect Object v9
-- DESCRIPTION: Will allow collection of an object. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=2(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Item collected"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> for collection sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_ResnodeCollected = {}

local collect_object = {}
local pickup_text = {}
local pickup_range = {}
local pickup_style = {}
local collected_text = {}
local prompt_display = {}

local tEnt = {}
local selectobj = {}

function collect_object_properties(e, pickup_text, pickup_range, pickup_style, collected_text, prompt_display)
	collect_object[e].pickup_text = pickup_text
	collect_object[e].pickup_range = pickup_range
	collect_object[e].pickup_style = pickup_style
	collect_object[e].collected_text = collected_text
	collect_object[e].prompt_display = prompt_display	
end

function collect_object_init(e)
	collect_object[e] = {}
	collect_object[e].pickup_text = "E to collect"
	collect_object[e].pickup_range = 80
	collect_object[e].pickup_style = 2
	collect_object[e].collected_text = "Item collected"
	collect_object[e].prompt_display = 1
	
	g_tEnt = 0 
	tEnt[e] = 0
	selectobj[e] = 0
end

function collect_object_main(e)
	local PlayerDist = GetPlayerDistance(e)
	
	if collect_object[e].pickup_style == 1 and PlayerDist < collect_object[e].pickup_range then
		if GetEntityCollectable(e) == 1 or GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then
				if collect_object[e].prompt_display == 1 then PromptLocal(e,collect_object[e].collected_text) end
				if collect_object[e].prompt_display == 2 then Prompt(collect_object[e].collected_text) end			
				PerformLogicConnections(e)
				SetEntityCollected(e,1)
				g_ResnodeCollected = e
				PlaySound(e,0)
			end
		end
	end
	
	if collect_object[e].pickup_style == 2 and PlayerDist < collect_object[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,collect_object[e].pickup_range,300)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	
		if PlayerDist < collect_object[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(tEnt[e]) == 1 or GetEntityCollectable(tEnt[e]) == 2 then
				if GetEntityCollected(tEnt[e]) == 0 then
					if collect_object[e].prompt_display == 1 then PromptLocal(e,collect_object[e].pickup_text) end
					if collect_object[e].prompt_display == 2 then Prompt(collect_object[e].pickup_text) end				
					if g_KeyPressE == 1 then				
						if collect_object[e].prompt_display == 1 then PromptLocal(e,collect_object[e].collected_text) end
						if collect_object[e].prompt_display == 2 then Prompt(collect_object[e].collected_text) end		
						PerformLogicConnections(e)
						SetEntityCollected(tEnt[e],1)
						g_ResnodeCollected = e
						PlaySound(e,0)
					end
				end
			end
		end	
	end	
end
