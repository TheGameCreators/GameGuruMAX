-- LUA Script - precede every function and global member with lowercase name of script + '_main'
-- Variable Switch v4: by Necrym59
-- DESCRIPTION: This object will be treated as a variable switch to change the state of an object or light.
-- DESCRIPTION: [PROMPT_TEXT$="+ to increase, - to decrease"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [USE_RANGE=90(1,200)]
-- DESCRIPTION: [#STARTER VALUE=0.0(0.0,100.0)] Base Starting value
-- DESCRIPTION: [#MINIMUM VALUE=0.0(0.0,100.0)] Minimum variable value
-- DESCRIPTION: [#MAXIMUM VALUE=100.0(1.0,1000.0)] Maximum variable value
-- DESCRIPTION: [VARIABLE_SWITCH_USER_GLOBAL$="Variable_Switch#1"] Unique User Global for this switch.
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None,1=Shape,2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: <Sound0> when the object is used by the player.

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local lower = string.lower
local varswitch 					= {}
local prompt_text 					= {}
local prompt_display 				= {}
local use_range 					= {}
local starter_value					= {}
local minimum_value					= {}
local maximum_value					= {}
local variable_switch_user_global	= {}
local item_highlight				= {}

local device_no			= {}
local doonce			= {}
local status			= {}
local switch_value 		= {}
local tEnt 				= {}
local selectobj 		= {}
local currentvalue		= {}

function variable_switch_properties(e, prompt_text, prompt_display, use_range, starter_value, minimum_value, maximum_value, variable_switch_user_global, item_highlight)
	varswitch[e].prompt_text = prompt_text or ""
	varswitch[e].prompt_display = prompt_display
	if use_range == nil then use_range = 100 end
	varswitch[e].use_range = use_range
	varswitch[e].starter_value = starter_value		
	varswitch[e].minimum_value = minimum_value	
	varswitch[e].maximum_value = maximum_value
	varswitch[e].variable_switch_user_global = variable_switch_user_global
	varswitch[e].item_highlight = item_highlight or 0
end

function variable_switch_init(e)
	varswitch[e] = {}
	varswitch[e].prompt_text = "E to use"
	varswitch[e].prompt_display = 1
	varswitch[e].use_range = 90
	varswitch[e].starter_value = 0		
	varswitch[e].minimum_value = 0
	varswitch[e].maximum_value = 100
	varswitch[e].variable_switch_user_global = ""
	varswitch[e].item_highlight = 0	
	
	switch_value[e] = 0
	
	device_no[e] = 0
	doonce[e] = 0
	status[e] = "init"
	tEnt[e]	= 0
	selectobj[e] = 0
	currentvalue[e] = 0
	g_tEnt = 0
end

function variable_switch_main(e)
	if status[e] == "init" then
		switch_value[e] = varswitch[e].starter_value
		if varswitch[e].variable_switch_user_global ~= "" then
			if _G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] end
			_G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] = switch_value[e]
		end		
		status[e] = "endinit"
	end

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist < varswitch[e].use_range and g_PlayerHealth > 0 then
		--pinpoint select object--
		module_misclib.pinpoint(e,varswitch[e].use_range,varswitch[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--
	end

	if PlayerDist < varswitch[e].use_range and tEnt[e] ~= 0 then
		if varswitch[e].prompt_display == 1 then PromptLocal(e,varswitch[e].prompt_text) end
		if varswitch[e].prompt_display == 2 then Prompt(varswitch[e].prompt_text) end
		if GetInKey() == "+" or GetInKey() == "=" then
			if varswitch[e].prompt_display == 1 then PromptLocal(e,"Setting: "..math.floor(switch_value[e]*10)/10) end
			if varswitch[e].prompt_display == 2 then Prompt("Setting: "..math.floor(switch_value[e]*10)/10) end
			SetAnimationName(e,"on")
			PlayAnimation(e)
			switch_value[e] = switch_value[e] + 0.02
			if switch_value[e] >= varswitch[e].maximum_value then switch_value[e] = varswitch[e].maximum_value end
			if varswitch[e].variable_switch_user_global ~= "" then
				if _G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] end
				_G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] = switch_value[e]
			end
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end
		end
		if GetInKey() == "-" or GetInKey() == "_" then
			if varswitch[e].prompt_display == 1 then PromptLocal(e,"Setting: "..math.floor(switch_value[e]*10)/10) end			
			if varswitch[e].prompt_display == 2 then Prompt("Setting: "..math.floor(switch_value[e]*10)/10) end
			SetAnimationName(e,"off")
			PlayAnimation(e)
			switch_value[e] = switch_value[e] - 0.02			
			if switch_value[e] <= varswitch[e].minimum_value then switch_value[e] = varswitch[e].minimum_value end
			if varswitch[e].variable_switch_user_global ~= "" then
				if _G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] ~= nil then currentvalue[e] = _G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] end
				_G["g_UserGlobal['"..varswitch[e].variable_switch_user_global.."']"] = switch_value[e]
			end
			if doonce[e] == 0 then
				PlaySound(e,0)
				doonce[e] = 1
			end
		end
	end
end
