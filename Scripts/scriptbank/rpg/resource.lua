-- Resource v6 by Necrym59
-- DESCRIPTION: The object will give the player a crafting resource. Object must be set to 'Collectable' and 'Resource'.
-- DESCRIPTION: [PICKUP_TEXT$="E to harvest"]
-- DESCRIPTION: [COLLECTED_TEXT$="Resource harvested"]
-- DESCRIPTION: [QUANTITY=1(1,5)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> for harvesting sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_ResnodeCollected = {}

local resource = {}
local pickup_text = {}
local collected_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local prompt_display = {}
local tEnt = {}
local selectobj = {}

function resource_properties(e, pickup_text, collected_text, quantity, pickup_range, pickup_style, prompt_display)
	resource[e].pickup_text = pickup_text
	resource[e].collected_text = collected_text
	resource[e].quantity = quantity
	resource[e].pickup_range = pickup_range
	resource[e].pickup_style = pickup_style
	resource[e].prompt_display = prompt_display	
end

function resource_init(e)
	resource[e] = {}
	resource[e].pickup_text = "E to harvest"
	resource[e].collected_text = "Resource harvested"
	resource[e].quantity = 1
	resource[e].pickup_range = 80
	resource[e].pickup_style = 1
	resource[e].prompt_display = 1		
	SetEntityQuantity(e,resource[e].quantity)	
	
	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
end

function resource_main(e)

	local PlayerDist = GetPlayerDistance(e)	
	
	if resource[e].pickup_style == 1 and PlayerDist < resource[e].pickup_range then
		if GetEntityCollectable(e) == 2 then
			if GetEntityCollected(e) == 0 then
				if resource[e].prompt_display == 1 then PromptLocal(e,resource[e].collected_text) end
				if resource[e].prompt_display == 2 then Prompt(resource[e].collected_text) end				
				PerformLogicConnections(e)
				PlaySound(e,0)
				SetEntityQuantity(e,resource[e].quantity)
				SetEntityCollected(e,1,-1)
				g_ResnodeCollected = e				
			end
		end
	end

	if resource[e].pickup_style == 2 and PlayerDist < resource[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,resource[e].pickup_range,300)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
		
		if PlayerDist < resource[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(e) == 2 then
				if GetEntityCollected(e) == 0 then
					if resource[e].prompt_display == 1 then PromptLocal(e,resource[e].pickup_text) end
					if resource[e].prompt_display == 2 then Prompt(resource[e].pickup_text) end
					if g_KeyPressE == 1 then
						if resource[e].prompt_display == 1 then PromptLocal(e,resource[e].collected_text) end
						if resource[e].prompt_display == 2 then Prompt(resource[e].collected_text) end
						PerformLogicConnections(e)
						PlaySound(e,0)
						SetEntityQuantity(e,resource[e].quantity)
						SetEntityCollected(e,1,-1)
						g_ResnodeCollected = e
					end
				end
			end
		end
	end	
end
