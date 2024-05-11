-- Switch_Sequence v2
-- DESCRIPTION: Will create a switch to an ActivateIfUsed link when completed in the correct order.
-- DESCRIPTION: [TOTAL_SWITCHES=1(1,5)] in group
-- DESCRIPTION: [SWITCH_NUMBER=1(1,5)] this switch number
-- DESCRIPTION: [SWITCH_RESET=5(1,20)] seconds before reset to off
-- DESCRIPTION: [USE_RANGE=60 (1,100)] distance to use
-- DESCRIPTION: [USE_PROMPT$="E to use"]
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None, 1=Shape, 2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: <Sound0> when switch activated
-- DESCRIPTION: <Sound1> when completed

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}
g_SwitchesSequenced = 0

local switch_seq 		= {}
local total_switches	= {}
local switch_number		= {}
local switch_reset		= {}
local use_range			= {}
local use_prompt		= {}
local message_duration	= {}
local item_highlight	= {}

local switch_seq_active 	= {}
local doonce 				= {}
local tEnt 					= {}
local selectobj 			= {}
local item_highlight		= {}
local reset					= {}
local completed				= {}

function switch_sequence_properties(e, total_switches, switch_number, switch_reset, use_range, use_prompt, message_duration, item_highlight)
	switch_seq[e].total_switches = total_switches
	switch_seq[e].switch_number = switch_number
	switch_seq[e].switch_reset = switch_reset
	switch_seq[e].use_range = use_range
	switch_seq[e].use_prompt = use_prompt
	switch_seq[e].message_duration = message_duration
	switch_seq[e].item_highlight = item_highlight or 0
end 

function switch_sequence_init(e)
	switch_seq[e] = {}
	switch_seq[e].total_switches = 1	
	switch_seq[e].switch_number = 1
	switch_seq[e].switch_reset = 5	
	switch_seq[e].use_range = 70
	switch_seq[e].use_prompt = "E to use"
	switch_seq[e].message_duration = 1
	switch_seq[e].item_highlight = 0
	
	g_SwitchesSequenced = 0
	doonce[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	reset[e] = math.huge
	completed[e] = 0
	selectobj[e] = 0
	--Set switch to off
	SetAnimationName(e,"off")
	PlayAnimation(e)					
	StopAnimation(e)
end
	 
function switch_sequence_main(e)		

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist <= switch_seq[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,switch_seq[e].use_range,switch_seq[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--			
	end	
	if PlayerDist <= switch_seq[e].use_range and tEnt[e] ~= 0 then
		Prompt(switch_seq[e].use_prompt)
		if g_KeyPressE == 1 then			
			if switch_seq[e].switch_number == g_SwitchesSequenced+1 then
				g_SwitchesSequenced = switch_seq[e].switch_number
				if g_SwitchesSequenced == switch_seq[e].total_switches and completed[e] == 0 then
					ActivateIfUsed(e)
					completed[e] = 1
				end	
			end
			if doonce[e] == 0 then 
				SetActivated(e,1)
				SetAnimationName(e,"on")
				PlayAnimation(e)					
				StopAnimation(e)
				PlaySound(e,0)
				PerformLogicConnections(e)
				reset[e] = g_Time + (switch_seq[e].switch_reset * 1000)
				doonce[e] = 1
			end
		end
	end
	if g_Time > reset[e] then
		if doonce[e] == 1 and g_SwitchesSequenced ~= switch_seq[e].total_switches then
			SetAnimationName(e,"off")
			PlayAnimation(e)					
			StopAnimation(e)
			PlaySound(e,0)
			PerformLogicConnections(e)
			SetActivated(e,0)
			g_SwitchesSequenced = 0
			doonce[e] = 0
		end	
	end	
end
