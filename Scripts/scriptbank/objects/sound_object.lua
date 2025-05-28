-- Sound Object v2 by Necrym59
-- DESCRIPTION: Will play a sound when interacted with and can activate Logic linked or IfUsed entities.
-- DESCRIPTION: Attach to an object
-- DESCRIPTION: [PROMPT_TEXT$="E to activate"] 
-- DESCRIPTION: [USE_RANGE=80(1,100)]
-- DESCRIPTION: [ACTIVATED_TEXT$="Activated"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)]
-- DESCRIPTION: [ACTIVATE_LOGIC!=0] if ON will activate logic links or IfUsed
-- DESCRIPTION: [SINGLE_USE!=0] if ON will be inactive after one use.
-- DESCRIPTION: <Sound0> Sound to play when activated.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local sound_object 		= {}
local prompt_text 		= {}
local use_range 		= {}
local activated_text 	= {}
local prompt_display 	= {}
local item_highlight 	= {}
local activate_logic 	= {}
local single_use		= {}

local tEnt = {}
local selectobj = {}
local doonce = {}

function sound_object_properties(e, prompt_text, use_range, activated_text, prompt_display, item_highlight, activate_logic, single_use)
	sound_object[e].prompt_text = prompt_text
	sound_object[e].use_range = use_range
	sound_object[e].activated_text = activated_text
	sound_object[e].prompt_display = prompt_display
	sound_object[e].item_highlight = item_highlight
	sound_object[e].activate_logic = activate_logic or 0
	sound_object[e].single_use = single_use or 0
end

function sound_object_init(e)
	sound_object[e] = {}
	sound_object[e].prompt_text = "E to activate"
	sound_object[e].use_range = 80
	sound_object[e].activated_text = "Activated"
	sound_object[e].prompt_display = 1
	sound_object[e].item_highlight = 0
	sound_object[e].activate_logic = 0
	sound_object[e].single_use = 0	

	g_tEnt = 0
	tEnt[e] = 0
	selectobj[e] = 0
	doonce[e] = 0
end

function sound_object_main(e)
	local PlayerDist = GetPlayerDistance(e)

	if PlayerDist < sound_object[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,sound_object[e].use_range,sound_object[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--

		if PlayerDist < sound_object[e].use_range and tEnt[e] ~= 0 and GetEntityVisibility(e) == 1 then
			if GetSoundPlaying(e,0) == 0 then	
				if sound_object[e].prompt_display == 1 then PromptLocal(e,sound_object[e].prompt_text) end
				if sound_object[e].prompt_display == 2 then Prompt(sound_object[e].prompt_text) end
			end	

			if g_KeyPressE == 1 and GetSoundPlaying(e,0) == 0 then	
				PlaySound(e,0)
				if sound_object[e].prompt_display == 1 then PromptLocal(e,sound_object[e].activated_text) end
				if sound_object[e].prompt_display == 2 then Prompt(sound_object[e].activated_text) end
				if sound_object[e].activate_logic == 1 then
					PerformLogicConnections(e)
					ActivateIfUsed(e)
				end
				if sound_object[e].single_use == 1 then
					SwitchScript(e,"no_behavior_selected.lua")
				end
			end
		end
	end
end
