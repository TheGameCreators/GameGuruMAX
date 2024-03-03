-- Recipe v5 by Necrym59 and Lee
-- DESCRIPTION: This object will give the player a crafting recipe. Object must be set to 'Collectable'.
-- DESCRIPTION: [PICKUP_TEXT$="E to collect"]
-- DESCRIPTION: [COLLECTED_TEXT$="Crafting Recipe collected"]
-- DESCRIPTION: [QUANTITY=1(1,5)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> for collected sound.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local recipe = {}
local pickup_text = {}
local collected_text = {}
local quantity = {}
local pickup_range = {}
local pickup_style = {}
local prompt_display = {}

local tEnt = {}
local selectobj = {}

function recipe_properties(e, pickup_text, collected_text, quantity, pickup_range, pickup_style, prompt_display)
	recipe[e].pickup_text = pickup_text
	recipe[e].collected_text = collected_text
	recipe[e].quantity = quantity
	recipe[e].pickup_range = pickup_range
	recipe[e].pickup_style = pickup_style
	recipe[e].prompt_display = prompt_display
end

function recipe_init(e)
	recipe[e] = {}
	recipe[e].pickup_text = "E to collect"
	recipe[e].collected_text = "Crafting Recipe collected"
	recipe[e].quantity = 1
	recipe[e].pickup_range = 80
	recipe[e].pickup_style = 2
	recipe[e].prompt_display = 1
	
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
end

function recipe_main(e)
	local PlayerDist = GetPlayerDistance(e)

	if recipe[e].pickup_style == 1 and PlayerDist < recipe[e].pickup_range then
		if GetEntityCollectable(e) == 1 then
			if GetEntityCollected(e) == 0 then	
				if recipe[e].prompt_display == 1 then PromptLocal(e,recipe[e].collected_text) end
				if recipe[e].prompt_display == 2 then Prompt(recipe[e].collected_text) end				
				PerformLogicConnections(e)
				PlaySound(e,0)
				SetEntityCollected(e,1,-1)			
			end
		end
	end
	
	if recipe[e].pickup_style == 2 and PlayerDist < recipe[e].pickup_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,recipe[e].pickup_range,300)
		tEnt[e] = g_tEnt
		--end pinpoint select object--		
		if PlayerDist < recipe[e].pickup_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetEntityCollectable(e) == 1 then
				if GetEntityCollected(e) == 0 then
					if recipe[e].prompt_display == 1 then PromptLocal(e,recipe[e].pickup_text) end
					if recipe[e].prompt_display == 2 then Prompt(recipe[e].pickup_text) end
					if g_KeyPressE == 1 then
						if recipe[e].prompt_display == 1 then PromptLocal(e,recipe[e].collected_text) end
						if recipe[e].prompt_display == 2 then Prompt(recipe[e].collected_text) end	
						PerformLogicConnections(e)
						PlaySound(e,0)
						SetEntityCollected(e,1,-1)
					end
				end
			end
		end
	end
end
