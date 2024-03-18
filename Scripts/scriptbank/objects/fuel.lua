-- Fuel v8
-- DESCRIPTION: The attached object will give the player a fuel resource if collected.
-- DESCRIPTION: [PROMPT_TEXT$="E to Collect"]
-- DESCRIPTION: [AMOUNT=5(1,30)]
-- DESCRIPTION: [PICKUP_RANGE=80(1,100)]
-- DESCRIPTION: [@PICKUP_STYLE=1(1=Automatic, 2=Manual)]
-- DESCRIPTION: [COLLECTED_TEXT$="Collected Fuel"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: Play the audio <Sound0> when picked up.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_fuel = {}

local fuel 				= {}
local prompt_text 		= {}
local amount 			= {}
local pickup_range 		= {}
local pickup_style 		= {}
local collected_text 	= {}
local prompt_display 	= {}

local total 			= {}
local tEnt 				= {}
local selectobj 		= {}		

function fuel_properties(e, prompt_text, amount, pickup_range, pickup_style, collected_text, prompt_display)
	fuel[e].prompt_text = prompt_text
	fuel[e].amount = amount
	fuel[e].pickup_range = pickup_range
	fuel[e].pickup_style = pickup_style
	fuel[e].collected_text = collected_text
	fuel[e].prompt_display = prompt_display	
end

function fuel_init_name(e)	
	fuel[e] = {}	
	fuel[e].prompt_text = "E to Collect"
	fuel[e].amount = 5
	fuel[e].pickup_range = 80
	fuel[e].pickup_style = 1
	fuel[e].collected_text = "Collected Fuel"
	fuel[e].prompt_display = 1	
	g_fuel = 0
	tEnt[e] = 0
	g_tEnt = 0 
	selectobj[e] = 0
end

function fuel_main(e)

	local PlayerDist = GetPlayerDistance(e)
	
	if fuel[e].pickup_style == 1 then
		if PlayerDist < fuel[e].pickup_range then
			if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].collected_text) end
			if fuel[e].prompt_display == 2 then Prompt(fuel[e].collected_text) end		
			PlaySound(e,0)
			PerformLogicConnections(e)		
			g_fuel = g_fuel + fuel[e].amount		
			Destroy(e)
		end
	end
	
	if fuel[e].pickup_style == 2 then
		if PlayerDist < fuel[e].pickup_range then
			--pinpoint select object--
			module_misclib.pinpoint(e,fuel[e].pickup_range,300)
			tEnt[e] = g_tEnt
			--end pinpoint select object--
		end
		if PlayerDist < fuel[e].pickup_range and tEnt[e] ~= 0 then
			if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].prompt_text) end
			if fuel[e].prompt_display == 2 then Prompt(fuel[e].prompt_text) end		
			if g_KeyPressE == 1 then				
				PlaySound(e,0)
				PerformLogicConnections(e)		
				g_fuel = g_fuel + fuel[e].amount
				if fuel[e].prompt_display == 1 then PromptLocal(e,fuel[e].collected_text) end
				if fuel[e].prompt_display == 2 then Prompt(fuel[e].collected_text) end
				Destroy(e)
			end
		end
	end
end
