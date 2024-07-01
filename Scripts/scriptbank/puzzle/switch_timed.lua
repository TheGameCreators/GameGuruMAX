-- Switch_Timed v2
-- DESCRIPTION: This object will be treated as a switch object for activating other objects or game elements with a timed reset delay.
-- DESCRIPTION: [USE_RANGE=60(1,100)] distance
-- DESCRIPTION: [USE_PROMPT$="E to use"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: [#RESET_DELAY=5.0(0.0,60.0)] seconds
-- DESCRIPTION: [@ITEM_HIGHLIGHT=0(0=None, 1=Shape, 2=Outline)] Use emmisive color for shape option
-- DESCRIPTION: <Sound0> when switch activates/deactivates

local module_misclib = require "scriptbank\\module_misclib"
local U = require "scriptbank\\utillib"
g_tEnt = {}

local switchtm 			= {}
local use_range			= {}
local use_prompt		= {}
local prompt_display	= {}
local reset_delay		= {}
local item_highlight	= {}

local doonce 			= {}
local swreset			= {}
local tEnt 				= {}
local selectobj 		= {}


function switch_timed_properties(e, use_range, use_prompt, prompt_display, reset_delay, item_highlight)
	switchtm[e].use_range = use_range
	switchtm[e].use_prompt = use_prompt
	switchtm[e].prompt_display = prompt_display
	switchtm[e].reset_delay = reset_delay
	switchtm[e].item_highlight = item_highlight or 0
end 

function switch_timed_init(e)
	switchtm[e] = {}
	switchtm[e].use_range = 70
	switchtm[e].use_prompt = "E to use"
	switchtm[e].prompt_display = 1
	switchtm[e].reset_delay = 20
	switchtm[e].item_highlight = 0	
	
	doonce[e] = 0
	tEnt[e] = 0
	g_tEnt = 0
	selectobj[e] = 0
	swreset[e] = math.huge
end
	 
function switch_timed_main(e)		

	local PlayerDist = GetPlayerDistance(e)
	if PlayerDist <= switchtm[e].use_range then
		--pinpoint select object--
		module_misclib.pinpoint(e,switchtm[e].use_range, switchtm[e].item_highlight)
		tEnt[e] = g_tEnt
		--end pinpoint select object--			
	end
	
	if PlayerDist <= switchtm[e].use_range and tEnt[e] ~= 0 then
		if switchtm[e].prompt_display == 1 then PromptLocal(e,switchtm[e].use_prompt) end
		if switchtm[e].prompt_display == 2 then Prompt(switchtm[e].use_prompt) end	
		if g_KeyPressE == 1 then
			swreset[e] = g_Time + (switchtm[e].reset_delay*1000)
			if doonce[e] == 0 then 
				SetAnimationName(e,"on")
				PlayAnimation(e)					
				StopAnimation(e)
				PlaySound(e,0)
				PerformLogicConnections(e)
				doonce[e] = 1
			end
		end			
	end
	if g_Time > swreset[e] then	
		if doonce[e] == 1 then 
			SetAnimationName(e,"off")
			PlayAnimation(e)					
			StopAnimation(e)
			PlaySound(e,0)
			PerformLogicConnections(e)
			doonce[e] = 0
		end
	end
end
