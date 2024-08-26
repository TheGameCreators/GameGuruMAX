-- Pipevalve v5 by Necrym59 
-- DESCRIPTION: This object will be treated as a switch object for activating other linked objects and/or IfUsed game element. Set Always Active = On
-- DESCRIPTION: [PROMPT_TEXT$="E to use"]
-- DESCRIPTION: [USE_RANGE=80(1,150)]
-- DESCRIPTION: [UNUSED_TEXT$="Valve wheel required"]
-- DESCRIPTION: [USED_TEXT$="Valve opened"]
-- DESCRIPTION: [@PROMPT_DISPLAY=1(1=Local,2=Screen)]
-- DESCRIPTION: <Sound0> when attaching.
-- DESCRIPTION: <Sound1> when turning
-- DESCRIPTION: <Sound2> when opened

local module_misclib = require "scriptbank\\module_misclib"
g_valve = {}

local pipevalve = {}
local prompt_text = {}
local use_range = {}
local unused_text = {}
local used_text = {}
local prompt_display = {}

local status = {}
local have_valve = {}
local activated = {}
local tusedvalue = {}
local use_item_now = {}
local pressed = {}

function pipevalve_properties(e, prompt_text, use_range, unused_text, used_text, prompt_display)
	pipevalve[e].prompt_text = prompt_text
	pipevalve[e].use_range = use_range
	pipevalve[e].unused_text = unused_text		
	pipevalve[e].used_text = used_text	
	pipevalve[e].prompt_display = prompt_display or 1
end 

function pipevalve_init(e)
	pipevalve[e] = {}
	pipevalve[e].prompt_text = "E to use"	
	pipevalve[e].use_range = 80
	pipevalve[e].unused_text = "Valve wheel required"		
	pipevalve[e].used_text = "Valve Opened"	
	pipevalve[e].prompt_display = 1	
	
	status[e] = "inactive"
	have_valve[e] = 0	
	activated[e] = 0
	tusedvalue[e] = 0
	use_item_now[e] = 0
	g_valve = 0
	pressed[e] = 0
end

function pipevalve_main(e)

	local PlayerDist = GetPlayerDistance(e)	
	if PlayerDist < pipevalve[e].use_range then
		if status[e] == "inactive" then  --Inactive	
			if g_valve > 0 then
				tusedvalue[e] = g_valve
				have_valve[e] = 1
			end
			if have_valve[e] == 1 then					
				SetAnimationName(e,"off")
				SetAnimationSpeed(e,1)					
				PlayAnimation(e)
				PlaySound(e,0)
				activated[e] = 0
				status[e] = "active"				
			end
			if have_valve[e] ~= 1 then	
				activated[e] = 0
				if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].unused_text) end
				if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].unused_text) end
				SetAnimationName(e,"idle")
				PlayAnimation(e)
				status[e] = "inactive"
			end
			if g_KeyPressE == 0 then pressed[e] = 0 end	
		end
		if status[e] == "active" then --Active
			if activated[e] == 1 then
				if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].used_text) end
				if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].used_text) end			
			end
			if activated[e] == 0 then
				--pinpoint select object--
				module_misclib.pinpoint(e,pipevalve[e].use_range,0)
				--end pinpoint select object--
				if pipevalve[e].prompt_display == 1 then PromptLocal(e,pipevalve[e].prompt_text) end
				if pipevalve[e].prompt_display == 2 then Prompt(pipevalve[e].prompt_text) end
			end
			if g_KeyPressE == 1 and activated[e] == 0 then		
				SetActivatedWithMP(e,201)				
				activated[e] = 1
				StopSound(e,0)
				PlaySound(e,1)				
				SetAnimationName(e,"on")
				SetAnimationSpeed(e,1)				
				PlayAnimation(e)				
				PerformLogicConnections(e)
				ActivateIfUsed(e)
				PlaySound(e,2)				
				if tusedvalue[e] > 0 then
					-- if this is a resource, it will deplete qty and set used to zero
					SetEntityUsed(e,tusedvalue[e]*-1)
					use_item_now[e] = 1
				end
				if use_item_now[e] == 1 then
					Destroy(g_valve) -- can only destroy resources that are qty zero
					g_valve = 0
				end
			end
		end
	end
end
